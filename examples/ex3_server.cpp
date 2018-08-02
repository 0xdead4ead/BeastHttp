#include <iostream>

#include <server.hpp>

using namespace std;



template<class Request>
auto make_response(const Request & req, const string & user_body){

    boost::beast::http::string_body::value_type body(user_body);

    auto const body_size = body.size();

    boost::beast::http::response<boost::beast::http::string_body> res{
         std::piecewise_construct,
         std::make_tuple(boost::move(body)),
         std::make_tuple(boost::beast::http::status::ok, req.version())};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.content_length(body_size);
    res.keep_alive(req.keep_alive());

    return res;

}


int main()
{
//g++ -c -std=gnu++17 -I.. -o ex3_server.o ./ex3_server.cpp
//g++ -o ex3_server ex3_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

//    root@x0x0:~# curl localhost --request 'GET' --request-target '/'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/about'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/cat'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/dog'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/mouse'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/ANY'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/alphabet/A'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/alphabet/B'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/alphabet/C'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/alphabet/ANY'
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    http::server my_http_server;

    auto animals_router = my_http_server.Router();
    auto alphabet_router = my_http_server.Router();

    animals_router.get("/cat", [](auto & req, auto & session){
        cout << req << endl; // /animals/cat
        session.do_write(boost::move(make_response(req, "Me-ow\n")));
    });

    animals_router.get("/dog", [](auto & req, auto & session){
        cout << req << endl; // /animals/dog
        session.do_write(boost::move(make_response(req, "Aw! Aw! Rrrrr\n")));
    });

    animals_router.get("/mouse", [](auto & req, auto & session){
        cout << req << endl; // /animals/mouse
        session.do_write(boost::move(make_response(req, "...\n")));
    });

    animals_router.all("/.*", [](auto & req, auto & session){
        cout << req << endl; // /animals/ANY
        session.do_write(boost::move(make_response(req, "???\n")));
    });

    my_http_server.use("/animals", animals_router);

    alphabet_router.get("/A", [](auto & req, auto & session){
        cout << req << endl; // /alphabet/A
        session.do_write(boost::move(make_response(req, "is A\n")));
    });

    alphabet_router.get("/B", [](auto & req, auto & session){
        cout << req << endl; // /alphabet/B
        session.do_write(boost::move(make_response(req, "is B\n")));
    });

    alphabet_router.get("/C", [](auto & req, auto & session){
        cout << req << endl; // /alphabet/C
        session.do_write(boost::move(make_response(req, "is C\n")));
    });

    alphabet_router.get("/.*", [](auto & req, auto & session){
        cout << req << endl; // /alphabet/ANY
        session.do_write(boost::move(make_response(req, "???\n")));
    });

    my_http_server.use("/alphabet", alphabet_router);

    my_http_server.get("/about", [](auto & req, auto & session){
        cout << req << endl; // /about
        session.do_write(boost::move(make_response(req, "About\n")));
    });

    my_http_server.get("/", [](auto & req, auto & session){
        cout << req << endl; // /
        session.do_write(boost::move(make_response(req, "Main\n")));
    });

    const auto & address = "127.0.0.1";
    auto port = 80;

    my_http_server.listen(address, port, [](auto & session){

        http::base::out("New client!!!");
        session.do_read();
    });

    cout << "Server starting on " << address << ':' << boost::lexical_cast<string>(port) << endl;

    http::base::processor::get().register_signals_handler([](int){
        http::base::processor::get().stop();
    }, std::vector<int>{SIGINT,SIGTERM, SIGQUIT});

    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

}
