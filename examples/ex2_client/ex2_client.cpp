#include <iostream>

#include <client.hpp>

using namespace std;


int main()
{
//g++ -c -std=gnu++14 -I../../include -o ex2_client.o ./ex2_client.cpp
//g++ -o ex2_client ex2_client.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n
    http::client instance;

    instance.on_connect = [](auto & session){
        http::base::out(session.getConnection()->stream().remote_endpoint().address().to_string() + " successful connected!");

        boost::beast::http::request<boost::beast::http::string_body> req;
        req.version(11); // HTTP 1.1
        req.method(boost::beast::http::verb::get); // GET
        req.target("/");
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        session.do_write(std::move(req));
    };

    instance.on_message = [](auto & res, auto & session){
        cout << res << endl;

        session.do_close();
        http::base::processor::get().stop();
    };

    instance.on_error = [](auto & error, auto & info){
        http::base::fail(error, info);
        cout << "Error of connect session!" << endl;
        http::base::processor::get().stop();
    };

    if(!instance.invoke("www.google.com", 80)){
        cout << "Failed to resolve address!" << endl;
        http::base::processor::get().stop();
    }


    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

    return 0;
}
