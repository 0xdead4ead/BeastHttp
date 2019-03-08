#include <reactor/listener.hxx>
#include <reactor/session.hxx>

#include <basic_router.hxx>
#include <out.hxx>

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
//    root@x0x0:~# (echo -en "GET /XXXX HTTP/1.1\nHost: localhost\n\nGET /XXXX HTTP/1.1\nHost: localhost\n\n"; sleep 0.1) | telnet localhost 80
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/XXXXXXXX'

    using namespace _0xdead4ead;

    using session_type = http::reactor::_default::session_type;
    using listener_type = http::reactor::_default::listener_type;

    http::basic_router<session_type> router;

    router.get(R"(^.*$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        // Received customer feedback! Sending an echo target answer. Launch timer again!
        context.get().send(make_response(request, request.target().to_string()), std::chrono::seconds(1)).launch_timer();
    });

    const auto& onError = [](auto code, auto from){
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", code.message());

        if (code == boost::system::errc::address_in_use)
            ioc.stop();
    };

    const auto& onTimer = [&](auto context){
        // Send End-of-stream. Write function can no longer be issued.
        context.get().eof();
        // Write half of the connection is closed
        // In this place connection point is closed gracefully
    };

    const auto& onAccept = [&](auto socket){
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, socket.remote_endpoint().address().to_string(), "connected!");

        // The client must send data no later than one seconds.
        session_type::recv(std::move(socket), std::chrono::seconds(1), router.resource_map(),
                           router.method_map(), boost::regex::ECMAScript, onError, onTimer).launch_timer();
    };

    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned short>(80);

    // Start accepting
    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string());
    listener_type::loop(ioc, {address, port}, onAccept, onError);

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
