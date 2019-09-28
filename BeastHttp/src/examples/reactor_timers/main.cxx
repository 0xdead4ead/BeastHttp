#include <http/reactor/listener.hxx>
#include <http/reactor/session.hxx>

#include <http/basic_router.hxx>
#include <http/out.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <thread>

namespace beast = boost::beast;

// Returns a success response (200)
template<class ResponseBody, class RequestBody>
auto make_200(const beast::http::request<RequestBody>& request,
              typename ResponseBody::value_type body,
              beast::string_view content)
{
    beast::http::response<ResponseBody> response{beast::http::status::ok, request.version()};
    response.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(beast::http::field::content_type, content);
    response.body() = body;
    response.prepare_payload();
    response.keep_alive(request.keep_alive());

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

    using HttpSession = http::reactor::_default::session_type;
    using HttpListener = http::reactor::_default::listener_type;

    http::basic_router<HttpSession> router{std::regex::ECMAScript};

    router.get(R"(^/echo$)", [](auto beast_http_request, auto context) {
        // Received customer feedback! Sending an echo target answer. Launch timer again!
        context.send(make_200<beast::http::string_body>(beast_http_request, beast_http_request.target().to_string(), "text/html"), std::chrono::seconds(3));
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
    const auto& onTimer = [&](auto context) {
        auto endpoint = context.asio_socket().remote_endpoint();

        http::out::prefix::version::time::pushn<std::ostream>(
                    out, endpoint.address().to_string() + ':' + std::to_string(endpoint.port()), "timed out!");

        // Send End-of-stream. Write function can no longer be issued.
        context.eof();
        // Write half of the connection is closed
        // In this place connection point is closed gracefully
    };

    // Handler incoming connections
    const auto& onAccept = [&](auto asio_socket) {
        auto endpoint = asio_socket.remote_endpoint();

        http::out::prefix::version::time::pushn<std::ostream>(
                    out, endpoint.address().to_string() + ':' + std::to_string(endpoint.port()), "connected!");

        // The client must send data no later than three seconds.
        HttpSession::recv(std::move(asio_socket), router, std::chrono::seconds(3), onError, onTimer);
    };

    auto const address = boost::asio::ip::address_v4::any();
    auto const port = static_cast<unsigned short>(8080);

    // Start accepting
    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string() + ':' + std::to_string(port));

    HttpListener::launch(ioc, {address, port}, onAccept, onError);

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
