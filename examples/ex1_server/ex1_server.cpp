#include <iostream>

#include <server.hpp>
#include <literal.hpp>

using namespace std;

template<class Request>
auto make_response(const Request & req, const string & user_body){

    boost::beast::http::string_body::value_type body(user_body);

    auto const body_size = body.size();

    boost::beast::http::response<boost::beast::http::string_body> res{
         std::piecewise_construct,
         std::make_tuple(std::move(body)),
         std::make_tuple(boost::beast::http::status::ok, req.version())};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.content_length(body_size);
    res.keep_alive(req.keep_alive());

    return res;

}

int main()
{
//g++ -c -std=gnu++14 -I../../include -o ex1_server.o ./ex1_server.cpp
//g++ -o ex1_server ex1_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

//    root@x0x0:~# (echo -en "GET /1 HTTP/1.1\nHost: localhost\n\nGET /2 HTTP/1.1\nHost: localhost\n\n"; sleep 0.1) | telnet localhost 80
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/1'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/2'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/a/b/c/d'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/user/date?y=1992&d=26&m=07'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/book/42'
//    root@x0x0:~# curl localhost --request 'POST' --request-target '/book/24'
//    root@x0x0:~# curl localhost --request 'PUT' --request-target '/book/12'

    http::server instance;

    using http::literal::operator""_get;
    using http::literal::operator""_all;

    "/user/date[?]y=(\\d+)&d=(\\d+)&m=(\\d+)"_get
            .assign(instance.param<int, int, int>()).with(
       [](auto & req, auto &, auto & next, auto &){
        // process '/user'
        cout << req << endl;
        next();
    }, [](auto & req, auto & session, auto & args){
        // process '/date'
        cout << req << endl;
        std::ostringstream os;
        os << "y = " << args._1 << " d = " << args._2 << " m = " << args._3 << std::endl;
        session.do_write(make_response(req, os.str()));
    });

    "/1"_get.assign(instance, [](auto & req, auto & session){
        cout << req << endl; // '/1'
        session.do_write(make_response(req, "GET 1\n"));
    });

    "/2"_get.assign(instance, [](auto & req, auto & session){
        cout << req << endl; // '/2'
        session.do_write(make_response(req, "GET 2\n"));
    });

    ".*"_all.assign(instance, [](auto & req, auto & session){
        cout << req << endl; // 'any'
        session.do_write(make_response(req, "error\n"));
    });

    "/a/b/c/d"_get.assign(instance,
       [](auto & req, auto & session, auto & next){

        cout << req << endl; // '/a'

        (void)session; next();
    }, [](auto & req, auto & session, auto & next){

        cout << req << endl; // '/b'

        (void)session; next();
    }, [](auto & req, auto & session, auto & next){

        cout << req << endl; // '/c'

        (void)session; next();
    }, [](auto & req, auto & session){

        cout << req << endl; // '/d'

        session.do_write(make_response(req, "ABCD\n"));
    });

    auto books_router = instance.ChainRouter();

    books_router.param<int>().route("/book/(\\d+)")
      .get([](auto & req, auto & session, auto & args){

        cout << req << endl;
        session.do_write(make_response(req, std::to_string(args._1) + " get\n"));

    }).post([](auto & req, auto & session, auto & args){

        cout << req << endl;
        session.do_write(make_response(req, std::to_string(args._1) + " post\n"));

    }).put([](auto & req, auto & session, auto & args){

        cout << req << endl;
        session.do_write(make_response(req, std::to_string(args._1) + " put\n"));

    });

    instance.use(books_router);

    const auto & on_accept = [](auto & session){
        http::base::out(session.getConnection().stream()
                        .remote_endpoint().address().to_string() + " connected");

        session.do_read();
    };

    const auto & on_error = [](auto & error, auto & info){
        cout << "Error code: " << error.value() << ", "
             << "Info: " << info << endl;
    };

    if(!instance.listen("127.0.0.1", 80, on_accept, on_error)){
        cout << "Failed to resolve address or can't open listener!" << endl;
        http::base::processor::get().stop();
    }

    http::base::processor::get().register_signals_handler([](int signal){
        if(signal == SIGINT)
            http::base::out("Interactive attention signal");
        else if(signal == SIGTERM)
            http::base::out("Termination request");
        else
            http::base::out("Quit");
        http::base::processor::get().stop();
    }, std::vector<int>{SIGINT,SIGTERM, SIGQUIT});

    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

    return 0;
}
