#include <reactor/listener.hxx>
#include <reactor/session.hxx>

#include <basic_router.hxx>

#include <out.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <thread>
#include <vector>

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
//    root@x0x0:~# (echo -en "GET /1 HTTP/1.1\nHost: localhost\n\nGET /2 HTTP/1.1\nHost: localhost\n\n"; sleep 0.1) | telnet localhost 80
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/1'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/2'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/3'

    using namespace _0xdead4ead;

    using session_type = http::reactor::_default::session_type;
    using listener_type = http::reactor::_default::listener_type;
    using session_array = std::vector<std::weak_ptr<session_type::flesh>>;

    const uint32_t max_clients = 100;
    const uint32_t max_threads = std::thread::hardware_concurrency() * 4;

    session_array clients;
    std::vector<std::thread> threads;

    clients.reserve(max_clients);
    threads.reserve(max_threads);

    http::basic_router<session_type> router;

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

        if (code == boost::system::errc::address_in_use)
            ioc.stop();
    };

    const auto& onAccept = [&](auto socket) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, socket.remote_endpoint().address().to_string(), "connected!");

        bool eof = false;

        if (clients.size() >= max_clients) {
            http::out::prefix::version::time::pushn<std::ostream>(
                        out, "Achieved maximum connections!", "Limit", max_clients);

            session_type::eof(std::move(socket), {}, {}, {}, onError);
            eof = true;
        }

        std::vector<session_array::const_iterator> expireds_;

        for (auto client = clients.cbegin(); client != clients.cend();) {
            if (client->expired())
                expireds_.push_back(client);

            client++;
        }

        for (auto expired : expireds_) {
            clients.erase(expired);
        }

        if (!eof) {
            std::weak_ptr<session_type::flesh> cl
                    = session_type::recv(std::move(socket), std::chrono::seconds(10), router.resource_map(),
                               router.method_map(), boost::regex::ECMAScript, onError).launch_timer().shared_from_this();

            clients.emplace_back(cl);
        }

        if (threads.size() < max_threads && clients.size() > std::pow(threads.size(), 2))
            threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                           (&boost::asio::io_context::run), std::ref(ioc)));
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

    threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                   (&boost::asio::io_context::run), std::ref(ioc)));

    for(auto& t : threads)
        t.join();

    return 0;
}
