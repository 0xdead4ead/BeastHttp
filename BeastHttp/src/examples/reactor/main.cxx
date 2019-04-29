#include <http/reactor/listener.hxx>
#include <http/reactor/session.hxx>

#include <http/basic_router.hxx>
#include <http/out.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <thread>

template<class Body>
auto make_response(const boost::beast::http::request<Body>& req,
                   const typename Body::value_type& user_body){

    typename Body::value_type body(user_body);

    auto const body_size = body.size();

    boost::beast::http::response<Body> res{
         std::piecewise_construct,
         std::make_tuple(std::move(body)),
         std::make_tuple(boost::beast::http::status::ok, req.version())};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.content_length(body_size);
    res.keep_alive(req.keep_alive());

    return res;
}

static boost::asio::io_context ioc;
static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDOUT_FILENO)};
static boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);

int main()
{
//    root@x0x0:~# (echo -en "GET /1 HTTP/1.1\nHost: localhost:8080\n\nGET /2 HTTP/1.1\nHost: localhost:8080\n\n"; sleep 0.1) | telnet localhost 8080
//    root@x0x0:~# curl localhost:8080/1 --request 'GET'
//    root@x0x0:~# curl localhost:8080/2 --request 'GET'
//    root@x0x0:~# curl localhost:8080/3 --request 'GET'

    using namespace _0xdead4ead;

    using HttpSession = http::reactor::_default::session_type;
    using HttpListener = http::reactor::_default::listener_type;

    http::basic_router<HttpSession> router{boost::regex::ECMAScript};

    router.get(R"(^/1$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 1\n"));
    });

    router.get(R"(^/2$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 2\n"));
    });

    router.get(R"(^/3$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 3\n"));
    });

    router.all(R"(^.*$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "ALL\n"));
    });

    const auto& onError = [](auto code, auto from) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", code.message());

        if (code == boost::system::errc::address_in_use or
                code == boost::system::errc::permission_denied)
            ioc.stop();
    };

    const auto& onAccept = [&](auto asioSocket) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, asioSocket.remote_endpoint().address().to_string(), "connected!");

        HttpSession::recv(std::move(asioSocket), router, onError);
    };

    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned short>(8080);

    // Start accepting
    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string());
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
