#include <iostream>

#include <server.hpp>

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
//g++ -c -std=gnu++17 -I.. -o ex3_server.o ./ex3_server.cpp
//g++ -o ex3_server ex3_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

//    root@x0x0:~# curl localhost --request 'GET' --request-target '/'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/about'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/cat'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/dog'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/mouse'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/any'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/audi'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/bmw'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/toyota'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/any'
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    http::server my_http_server;

    auto animals_router = my_http_server.Router();
    auto alphabet_router = my_http_server.Router();

    animals_router.get("/cat[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals/cat' or '/animals/cat/'
        session.do_write(std::move(make_response(req, "me-ow\n")));
    });

    animals_router.get("/dog[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals/dog' or '/animals/dog/'
        session.do_write(std::move(make_response(req, "aw! aw! Rrrrr\n")));
    });

    animals_router.get("/mouse[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals/mouse' or '/animals/mouse/'
        session.do_write(std::move(make_response(req, "...\n")));
    });

    animals_router.get("[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals' or '/animals/'
        session.do_write(std::move(make_response(req, "animals\n")));
    });

    my_http_server.use("/animals", animals_router);

    cars_router.get("/audi[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars/audi' or '/cars/audi/'
        session.do_write(std::move(make_response(req, "audi\n")));
    });

    cars_router.get("/bmw[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars/bmw' or '/cars/bmw/'
        session.do_write(std::move(make_response(req, "bmw\n")));
    });

    cars_router.get("/toyota[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars/toyota' or '/cars/toyota/'
        session.do_write(std::move(make_response(req, "toyota\n")));
    });

    cars_router.get("[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars' or '/cars/'
        session.do_write(std::move(make_response(req, "cars\n")));
    });

    my_http_server.use("/cars[/]??", cars_router);

    my_http_server.get("/about[/]??", [](auto & req, auto & session){
        cout << req << endl; // /about
        session.do_write(std::move(make_response(req, "About\n")));
    });

    my_http_server.get("/", [](auto & req, auto & session){
        cout << req << endl; // /
        session.do_write(std::move(make_response(req, "Main\n")));
    });

    my_http_server.all("/.*", [](auto & req, auto & session){
        cout << req << endl; // /any
        session.do_write(std::move(make_response(req, "Error\n")));
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
