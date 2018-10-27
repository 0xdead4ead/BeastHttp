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
//g++ -c -std=gnu++14 -I../../include -o ex3_server.o ./ex3_server.cpp
//g++ -o ex3_server ex3_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

//    root@x0x0:~# curl localhost --request 'GET' --request-target '/'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/about'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/cat'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/dog'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals/mouse'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/animals'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/audi'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/bmw'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars/toyota'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/cars'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/books/book'
//    root@x0x0:~# curl localhost --request 'POST' --request-target '/books/book'
//    root@x0x0:~# curl localhost --request 'PUT' --request-target '/books/book'
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    http::server instance;

    auto animals_router = instance.BasicRouter();
    auto cars_router = instance.BasicRouter();
    auto books_router = instance.ChainRouter();

    animals_router.get("/cat[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals/cat' or '/animals/cat/'
        session.do_write(make_response(req, "me-ow\n"));
    });

    animals_router.get("/dog[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals/dog' or '/animals/dog/'
        session.do_write(make_response(req, "aw! aw! Rrrrr\n"));
    });

    animals_router.get("/mouse[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals/mouse' or '/animals/mouse/'
        session.do_write(make_response(req, "...\n"));
    });

    animals_router.get("[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/animals' or '/animals/'
        session.do_write(make_response(req, "animals home page\n"));
    });

    cars_router.get("/audi[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars/audi' or '/cars/audi/'
        session.do_write(make_response(req, "audi\n"));
    });

    cars_router.get("/bmw[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars/bmw' or '/cars/bmw/'
        session.do_write(make_response(req, "bmw\n"));
    });

    cars_router.get("/toyota[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars/toyota' or '/cars/toyota/'
        session.do_write(make_response(req, "toyota\n"));
    });

    cars_router.get("[/]??", [](auto & req, auto & session){
        cout << req << endl; // '/cars' or '/cars/'
        session.do_write(make_response(req, "cars home page\n"));
    });

    instance.use("/animals", animals_router);
    instance.use("/cars", cars_router);

    books_router.route("/book[/]??")
      .get([](auto & req, auto & session){

        cout << req << endl;
        session.do_write(make_response(req, "get a random book\n"));

    }).post([](auto & req, auto & session){

        cout << req << endl;
        session.do_write(make_response(req, "add a book\n"));

    }).put([](auto & req, auto & session){

        cout << req << endl;
        session.do_write(make_response(req, "update the book\n"));

    });

    instance.use("/books", books_router);

    instance.get("/about[/]??", [](auto & req, auto & session){
        cout << req << endl; // /about
        session.do_write(make_response(req, "about\n"));
    });

    instance.get("/", [](auto & req, auto & session){
        cout << req << endl; // /
        session.do_write(make_response(req, "main home page\n"));
    });

    instance.all(".*", [](auto & req, auto & session){
        cout << req << endl; // /any
        session.do_write(make_response(req, "error\n"));
    });

    const auto & address = "127.0.0.1";
    uint32_t port = 80;

    instance.listen(address, port, [](auto & session){
        http::base::out(session.getConnection()->stream().remote_endpoint().address().to_string() + " connected");
        session.do_read();
    });

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

}
