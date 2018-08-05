#include <iostream>

#include <client.hpp>

using namespace std;


int main()
{
//g++ -c -std=gnu++17 -I.. -o ex2_client.o ./ex2_client.cpp
//g++ -o ex2_client ex2_client.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n
    http::client my_http_client;

    const auto & host = "www.google.com";
    uint32_t port = 80;

    const auto & on_connect = [&host](auto & session){
        http::base::out("Successful connected!");

        boost::beast::http::request<boost::beast::http::string_body> req;
        req.version(11); // HTTP 1.1
        req.method(boost::beast::http::verb::get); // GET
        req.target("/");
        req.set(boost::beast::http::field::host, host);
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        session.do_write(std::move(req));
    };

    const auto & on_receive = [](auto & res, auto & session){
        cout << res << endl;
        session.do_close();
        http::base::processor::get().stop();
    };

    my_http_client.invoke(host, port, on_connect, on_receive);


    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

    return 0;
}
