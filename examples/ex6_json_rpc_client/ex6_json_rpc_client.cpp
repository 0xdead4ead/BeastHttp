// beast-http server JSON-RPC 2.0 communication demo
// Official document:
// http://www.ietf.org/rfc/rfc4627.txt

// HTTP Specification:
// http://www.ietf.org/rfc/rfc2616.txt

#include <iostream>
#include <client.hpp>
#include <array>

// Copyright (c) 2018 0xdead4ead

// This is an example uses 'JSON for Modern C++'
// Github Repository: https://github.com/nlohmann/json
#include <nlohmann/json/single_include/nlohmann/json.hpp>
#include <nlohmann/fifo_map/src/fifo_map.hpp>

// https://github.com/nlohmann/json/issues/485#issuecomment-333652309
// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template<class K, class V, class dummy_compare, class A>
using workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using json = nlohmann::basic_json<workaround_fifo_map>;


int main()
{
//g++ -c -std=gnu++14 -I../../include -I../../extern -o ex6_json_rpc_client.o ./ex6_json_rpc_client.cpp
//g++ -o ex6_json_rpc_client ex6_json_rpc_client.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

    //##################################################################
    // Before start this app, run an ex5_json_rpc_server!
    http::client my_http_client;

    const auto & host = "127.0.0.1";
    uint32_t port = 80;

    const auto & on_connect = [](auto & session){
        http::base::out("Success connect!");

            int a = 20, b = 10;

            // Make batch json reqest
            std::array<json,4> batch;
            batch[0]["jsonrpc"] = "2.0";
            batch[0]["method"] = "add";
            batch[0]["params"] = {a,b};
            batch[0]["id"] = 1;
            batch[1]["jsonrpc"] = "2.0";
            batch[1]["method"] = "sub";
            batch[1]["params"] = {a,b};
            batch[1]["id"] = 2;
            batch[2]["jsonrpc"] = "2.0";
            batch[2]["method"] = "mul";
            batch[2]["params"] = {a,b};
            batch[2]["id"] = 3;
            batch[3]["jsonrpc"] = "2.0";
            batch[3]["method"] = "div";
            batch[3]["params"] = {a,b};
            batch[3]["id"] = 4;

            json j_rpc_req;
            for(const auto & j_req : batch)
                j_rpc_req.push_back(j_req);


        boost::beast::http::request<boost::beast::http::string_body> req;
        req.version(11);
        req.method(boost::beast::http::verb::post);
        req.target("/rpc");
        req.body() = j_rpc_req.dump();
        req.set(boost::beast::http::field::content_type, "application/json");
        req.prepare_payload();
        session.do_write(std::move(req));
    };

    const auto & on_receive = [](auto & res, auto & session){
        std::cout << res << std::endl;
        session.do_close();
        http::base::processor::get().stop();
    };

    my_http_client.invoke(host, port, on_connect, on_receive);


    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

}
