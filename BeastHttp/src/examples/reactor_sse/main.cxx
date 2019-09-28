#include <http/reactor/listener.hxx>
#include <http/reactor/session.hxx>

#include <http/basic_router.hxx>
#include <http/out.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <thread>

namespace beast = boost::beast;

template<class ResponseBody>
auto make_200event(typename ResponseBody::value_type body,
                   unsigned version)
{
    beast::http::response<ResponseBody> response{beast::http::status::ok, version};
    response.set(beast::http::field::connection, "keep-alive");
    response.set(beast::http::field::cache_control, "no-cache");
    response.set(beast::http::field::content_type, "text/event-stream");
    response.set(beast::http::field::access_control_allow_origin, "*");
    response.body() = body;

    return response;
}

// Returns a not found response (404)
template<class ResponseBody, class RequestBody>
auto make_404(const beast::http::request<RequestBody>& request,
              typename ResponseBody::value_type body,
              beast::string_view content)
{
    beast::http::response<ResponseBody> response{beast::http::status::not_found, request.version()};
    response.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(beast::http::field::content_type, content);
    response.body() = body;
    response.prepare_payload();
    response.keep_alive(request.keep_alive());

    return response;
}

static boost::asio::io_context ioc;
static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDERR_FILENO)};
static boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);

int main()
{
    using namespace _0xdead4ead;

    using http_session = http::reactor::_default::session_type;
    using http_listener = http::reactor::_default::listener_type;

    http::basic_router<http_session> router{std::regex::ECMAScript};

    // Set router targets
    router.get(R"(^/sse$)", [](auto /*beast_http_request*/, auto context) {
        // Begin send server event after one seconds
        context.wait(std::chrono::seconds(1));
    });

    router.all(R"(^.*$)", [](auto beast_http_request, auto context) {
        context.send(make_404<beast::http::string_body>(beast_http_request, "Resource is not found\n", "text/html"));
    });

    // Error and warning handler
    const auto& onError = [](auto system_error_code, auto from) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", system_error_code.message());

        // I/O context will be stopped, if code value is EADDRINUSE or EACCES
        if (system_error_code == boost::system::errc::address_in_use or
                system_error_code == boost::system::errc::permission_denied)
            ioc.stop();
    };

    // Timer expire handler
    auto onTimer = [i = 0](auto context) mutable {
        if (i++ < 5) {
            std::ostringstream os;
            os << "event: datetime\ndata: " << boost::posix_time::second_clock::local_time() << " id = " << i << "\n\n";

            // Send event every 3 seconds or so forever...
            context.push(make_200event<beast::http::string_body>(os.str(), 11));
            context.wait(std::chrono::seconds(3));
            return;
        }

        context.eof();
    };

    // Handler incoming connections
    const auto& onAccept = [&](auto asio_socket) {
        auto endpoint = asio_socket.remote_endpoint();

        http::out::prefix::version::time::pushn<std::ostream>(
                    out, endpoint.address().to_string() + ':' + std::to_string(endpoint.port()), "connected!");

        // Start receive HTTP request
        http_session::recv(std::move(asio_socket), router, onError, onTimer);
    };

    auto const address = boost::asio::ip::address_v4::any();
    auto const port = static_cast<unsigned short>(8080);

    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string() + ':' + std::to_string(port));

    // Start accepting
    http_listener::launch(ioc, {address, port}, onAccept, onError);

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    sig_set.async_wait([](boost::system::error_code const&, int sig) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "Capture", sig == SIGINT ? "SIGINT." : "SIGTERM.", "Stop!");
        ioc.stop();
    });

    uint32_t pool_size = std::thread::hardware_concurrency() * 2;

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> threads;
    threads.reserve(pool_size > 0 ? pool_size : 4);
    for(uint32_t i = 0; i < pool_size; i++)
        threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                       (&boost::asio::io_context::run), std::ref(ioc)));

    // Block until all the threads exit
    for(auto& t : threads)
        t.join();

    return 0;
}
