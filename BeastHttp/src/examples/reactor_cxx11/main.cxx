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
beast::http::response<ResponseBody>
make_200(const beast::http::request<RequestBody>& request,
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
beast::http::response<ResponseBody>
make_404(const beast::http::request<RequestBody>& request,
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

    using http_context = typename http_session::context_type;
    using beast_http_request = typename http_session::request_type;
    using asio_socket = typename http_session::socket_type;

    http::basic_router<http_session> router{std::regex::ECMAScript};

    router.get(R"(^/$)", [](beast_http_request r, http_context c) {
        // Send content message to client and wait to receive next request
        c.send(make_200<beast::http::string_body>(r, "Main page\n", "text/html"));
    });

    router.all(R"(^.*$)", [](beast_http_request r, http_context c) {
        c.send(make_404<beast::http::string_body>(r, "Resource is not found\n", "text/html"));
    });

    // Error and warning handler
    const auto& onError = [](boost::system::error_code code, boost::string_view from) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", code.message());

        // I/O context will be stopped, if code value is EADDRINUSE or EACCES
        if (code == boost::system::errc::address_in_use or
                code == boost::system::errc::permission_denied)
            ioc.stop();
    };

    // Handler incoming connections
    const auto& onAccept = [&](asio_socket s) {
        auto endpoint = s.remote_endpoint();

        http::out::prefix::version::time::pushn<std::ostream>(
                    out, endpoint.address().to_string() + ':' + std::to_string(endpoint.port()), "connected!");

        // Start receive HTTP request
        http_session::recv(std::move(s), router, onError);
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
