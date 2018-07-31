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
//g++ -c -std=gnu++17 -I.. -o ex1_server.o ./ex1_server.cpp
//g++ -o ex1_server ex1_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

//    root@x0x0:~# curl localhost --request 'GET' --request-target '/1'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/2'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/ANY/REQUEST'
    http::server my_http_server;

    my_http_server.route("^/1$", http::method_t::get, [](auto & req, auto & session){

       cout << req << endl;

       session.do_write(boost::move(make_response(req, "GET 1\n")));

    });

    my_http_server.route("^/2$", http::method_t::get, [](auto & req, auto & session){

       cout << req << endl;

       session.do_write(boost::move(make_response(req, "GET 2\n")));

    });

    my_http_server.all_route([](auto & req, auto & session){

        cout << req << endl;

        session.do_write(boost::move(make_response(req, "GET any\n")));

    });

    my_http_server.listen("127.0.0.1",80, [](auto & session){

        http::base::out("New client!!!");
        session.do_read();
    });

    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

    return 0;
}
