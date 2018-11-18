// beast-http server JSON-RPC 2.0 communication demo
// Official document:
// http://www.ietf.org/rfc/rfc4627.txt

// HTTP Specification:
// http://www.ietf.org/rfc/rfc2616.txt

#include <iostream>
#include <server.hpp>
#include <boost/lexical_cast.hpp>

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

// https://www.simple-is-better.org/json-rpc/transport_http.html
// 200 OK - for responses (both for Response and Error objects)
// 204 No Response / 202 Accepted - for empty responses, i.e. as response to a Notification
// 307 Temporary Redirect / 308 Permanent Redirect - for HTTP-redirections (note that the request may not be automatically retransmitted)
// 405 Method Not Allowed
//      if HTTP GET is not supported: for all HTTP GET requests
//      if HTTP GET is supported: if the client tries to call a non-safe/non-indempotent method via HTTP GET
// 415 Unsupported Media Type - if the Content-Type is not application/json
// others - for HTTP-errors or HTTP-features outside of the scope of JSON-RPC

template<class Request>
auto make_200(const Request & req, const std::string & user_body){ // OK

    boost::beast::http::string_body::value_type body(user_body);

    auto const body_size = body.size();

    boost::beast::http::response<boost::beast::http::string_body> res{
         std::piecewise_construct,
         std::make_tuple(std::move(body)),
         std::make_tuple(boost::beast::http::status::ok, req.version())};

    res.set(boost::beast::http::field::content_type, "application/json");
    res.keep_alive(false);
    res.content_length(body_size);

    return res;
}

template<class Request>
auto make_204(const Request & req){ // No Responce

    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::no_content,
                req.version()};

    res.keep_alive(false);
    return res;
}

template<class Request>
auto make_400(const Request & req){ // Bad Request

    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::bad_request,
                req.version()};

    res.keep_alive(false);
    return res;
}

template<class Request>
auto make_411(const Request & req){ // Length Required

    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::length_required,
                req.version()};

    res.keep_alive(false);
    return res;
}

template<class Request>
auto make_415(const Request & req){ // Unsupported Media Type

    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::unsupported_media_type,
                req.version()};

    res.keep_alive(false);
    return res;
}

template<class Request>
auto make_406(const Request & req){ // Not Acceptable

    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::not_acceptable,
                req.version()};

    res.set(boost::beast::http::field::content_type, "application/json");
    res.keep_alive(false);
    return res;
}

template<class Request>
auto make_405(const Request & req){ // Method Not Allowed

    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::method_not_allowed,
                req.version()};

    res.set(boost::beast::http::field::allow, "POST");
    res.keep_alive(false);
    return res;
}

//code	                message	         meaning
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//-32700	            Parse error	     Invalid JSON was received by the server.
//                                       An error occurred on the server while parsing the JSON text.
//-32600	            Invalid Request	 The JSON sent is not a valid Request object.
//-32601	            Method not found The method does not exist / is not available.
//-32602	            Invalid params	 Invalid method parameter(s).
//-32603	            Internal error	 Internal JSON-RPC error.
//-32000 to -32099	    Server error	 Reserved for implementation-defined server-errors.
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

template<class Result>
auto json_rpc(const Result & result, const uint32_t id){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["result"] = result;
    j_rpc_res["id"] = id;

    return j_rpc_res;
}

auto json_rpc_32000(const uint32_t id){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["error"]["code"] = -32000;
    j_rpc_res["error"]["message"] = "Server error";
    j_rpc_res["id"] = id;

    return j_rpc_res;
}

auto json_rpc_32700(){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["error"]["code"] = -32700;
    j_rpc_res["error"]["message"] = "Parse error";
    j_rpc_res["id"] = nullptr;

    return j_rpc_res;
}

auto json_rpc_32600(){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["error"]["code"] = -32600;
    j_rpc_res["error"]["message"] = "Invalid Request";
    j_rpc_res["id"] = nullptr;

    return j_rpc_res;
}

auto json_rpc_32601(const uint32_t id){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["error"]["code"] = -32601;
    j_rpc_res["error"]["message"] = "Method not found";
    j_rpc_res["id"] = id;

    return j_rpc_res;
}

auto json_rpc_32603(){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["error"]["code"] = -32603;
    j_rpc_res["error"]["message"] = "Internal error";
    j_rpc_res["id"] = nullptr;

    return j_rpc_res;
}

auto json_rpc_32602(const uint32_t id){
    json j_rpc_res;
    j_rpc_res["jsonrpc"] = "2.0";
    j_rpc_res["error"]["code"] = -32602;
    j_rpc_res["error"]["message"] = "Invalid params";
    j_rpc_res["id"] = id;

    return j_rpc_res;
}

struct calc{

    static int add(const int a, const int b){
        return a + b;
    }

    static int sub(const int a, const int b){
        return a - b;
    }

    static int mul(const int a, const int b){
        return a * b;
    }

    static int div(const int a, const int b){
        if(b != 0)
            return a / b;
        throw std::runtime_error("div 0");
    }
};

// one request
json json_rpc_process(const json & j_rpc_req){
    try {
        // check must jsonrpc = '2.0'
        if(j_rpc_req.at("jsonrpc") != "2.0"){
            return json_rpc_32603();
        }

    } catch (json::type_error &) {
        return json_rpc_32600();
    } catch (json::out_of_range &) {
        return json_rpc_32600();
    }

    // id
    uint32_t id;
    bool has_notify = false;

    try {
        id = j_rpc_req.at("id");
    } catch (json::type_error &) {
        return json_rpc_32600();
    } catch (json::out_of_range &) {
        //This is a Notification request
        //The response to the client will not be sent.
        has_notify = true;
    }

    int (*calc_f)(const int, const int) = nullptr;

    // method
    try {
        if(j_rpc_req.at("method") == "add")
            calc_f = &calc::add;
        else if(j_rpc_req.at("method") == "sub")
            calc_f = &calc::sub;
        else if(j_rpc_req.at("method") == "mul")
            calc_f = &calc::mul;
        else if(j_rpc_req.at("method") == "div")
            calc_f = &calc::div;
        else{
            return json_rpc_32601(id);
        }
    } catch (json::type_error &) {
        return json_rpc_32600();
    } catch (json::out_of_range &){
        return json_rpc_32600();
    }

    int result;
    json params;

    // params
    try {
        params = j_rpc_req.at("params");
    } catch (json::type_error &) {
        return json_rpc_32600();
    } catch (json::out_of_range &) {
        return json_rpc_32600();
    }

    try {
        result = calc_f(params.at(0), params.at(1)); // call a method
    } catch (json::type_error &) {
        return json_rpc_32602(id);
    } catch (json::out_of_range &) {
        return json_rpc_32602(id);
    } catch (std::runtime_error &) {
        return json_rpc_32000(id);
    }

    if(!has_notify)
        return json_rpc(result, id);

    return {};
}

// many requests
json json_rpc_batch_process(const json & j_rpc_req){
    json result_batch{};
    for (auto it = j_rpc_req.begin(); it != j_rpc_req.end(); ++it) {

        try {
            // check must jsonrpc = '2.0'
            if((*it).at("jsonrpc") != "2.0"){
                result_batch.push_back(json_rpc_32603());
                continue;
            }

        } catch (json::type_error &) {
            result_batch.push_back(json_rpc_32600());
            continue;
        } catch (json::out_of_range &) {
            result_batch.push_back(json_rpc_32600());
            continue;
        }

        // id
        uint32_t id;
        bool has_notify = false;

        try {
            id = (*it).at("id");
        } catch (json::type_error &) {
            result_batch.push_back(json_rpc_32600());
            continue;
        } catch (json::out_of_range &) {
            //This is a Notification request
            //The response to the client will not be sent.
            has_notify = true;
        }

        int (*calc_f)(const int, const int) = nullptr;

        // method
        try {
            if((*it).at("method") == "add")
                calc_f = &calc::add;
            else if((*it).at("method") == "sub")
                calc_f = &calc::sub;
            else if((*it).at("method") == "mul")
                calc_f = &calc::mul;
            else if((*it).at("method") == "div")
                calc_f = &calc::div;
            else{
                result_batch.push_back(json_rpc_32601(id));
                continue;
            }
        } catch (json::type_error &) {
            result_batch.push_back(json_rpc_32600());
            continue;
        } catch (json::out_of_range &){
            result_batch.push_back(json_rpc_32600());
            continue;
        }

        int result;
        json params;

        // params
        try {
            params = (*it).at("params");
        } catch (json::type_error &) {
            result_batch.push_back(json_rpc_32600());
            continue;
        } catch (json::out_of_range &) {
            result_batch.push_back(json_rpc_32600());
            continue;
        }

        try {
            result = calc_f(params.at(0), params.at(1)); // call a method
        } catch (json::type_error &) {
            result_batch.push_back(json_rpc_32602(id));
            continue;
        } catch (json::out_of_range &) {
            result_batch.push_back(json_rpc_32602(id));
            continue;
        } catch (std::runtime_error &) {
            result_batch.push_back(json_rpc_32000(id));
            continue;
        }

        if(!has_notify)
            result_batch.push_back(json_rpc(result, id));

    }

    return result_batch;
}

int main()
{
//g++ -c -std=gnu++14 -I../../include -I../../extern -o ex5_json_rpc_server.o ./ex5_json_rpc_server.cpp
//g++ -o ex5_json_rpc_server ex5_json_rpc_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n

    /*
      curl localhost --request 'POST' --request-target '/rpc' \
      --data-binary '{"jsonrpc":"2.0","method":"add","params":[23,42], "id": 1}'  \
      -H 'Content-Type:application/json' -H 'Accept:application/json'
    */
    /*
       curl localhost --request 'POST' --request-target '/rpc' \
       --data-binary '{"jsonrpc":"2.0","method":"sub","params":[42,23], "id": 1}'  \
       -H 'Content-Type:application/json' -H 'Accept:application/json'
    */
    /*
       curl localhost --request 'POST' --request-target '/rpc' \
       --data-binary '{"jsonrpc":"2.0","method":"mul","params":[4,5], "id": 1}'  \
       -H 'Content-Type:application/json' -H 'Accept:application/json'
    */
    /*
       curl localhost --request 'POST' --request-target '/rpc' \
       --data-binary '{"jsonrpc":"2.0","method":"div","params":[9,3], "id": 1}'  \
       -H 'Content-Type:application/json' -H 'Accept:application/json'
    */
    //##################################################################
    http::server instance;

    instance.post("/rpc", [](auto & req, auto & session){

        // Check fields of header

        // A HTTP POST request MUST specify the following headers:
        //auto content_type = boost::beast::string_view{};
        //auto content_length = boost::beast::string_view{};
        //auto accept = boost::beast::string_view{};

        // If a request contains a message-body and a Content-Length is not given, the server SHOULD respond with 400 (bad request)
        try {
            if(boost::lexical_cast<size_t>(req.at(boost::beast::http::field::content_length)) != req.body().size()) // must contain the correct length
                return session.do_write(make_411(req));
        } catch (const std::out_of_range &) {
            return session.do_write(make_400(req));
        }

        // If a field Content-Type is missing, server respond 415 (Unsupported Media Type)
        try {
            if(req.at(boost::beast::http::field::content_type) != "application/json") // must be application/json
                return session.do_write(make_415(req));
        } catch (const std::out_of_range &) {
            return session.do_write(make_400(req));
        }

        // a field Accept is optional
        try {
            if(req.at(boost::beast::http::field::accept) != "application/json") // must be application/json (optional)
                return session.do_write(make_406(req));

        } catch (const std::out_of_range &) {
            // it does not matter
        }

        // Start json parcing!
        json j_rpc_req;

        try
        {
            j_rpc_req = json::parse(req.body());
        } catch (json::parse_error &) {
            return session.do_write(make_200(req, json_rpc_32700().dump()));
        }

        // json-rpc array?
        if(j_rpc_req.is_array()){
            //Iteratin batch
            auto result_batch = json_rpc_batch_process(j_rpc_req);

            if(result_batch.size() != 0)
                return session.do_write(make_200(req, result_batch.dump()));
            return session.do_write(make_204(req));
        }

        // json request is a object
        auto result = json_rpc_process(j_rpc_req);

        if(result.size() != 0)
            return session.do_write(make_200(req, result.dump()));
        return session.do_write(make_204(req));
    });

    instance.all(".*", [](auto & req, auto & session){
        return session.do_write(make_405(req));
    });

    const auto & on_accept = [](auto & session){
        http::base::out(session.getConnection().stream().remote_endpoint().address().to_string() + " connected");
        session.do_read();
    };

    const auto & on_error = [](auto & /*error*/){
        //std::cout << "Process an error is " << error.value() << endl;
    };

    if(!instance.listen("127.0.0.1", 80, on_accept, on_error)){
        std::cout << "Failed to resolve address or can't open listener!" << std::endl;
        http::base::processor::get().stop();
    }

    //##################################################################

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
