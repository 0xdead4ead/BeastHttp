#include <iostream>

#include <client.hpp>

using namespace std;

int main()
{

    auto ec = boost::system::error_code{};
    auto endpoint = http::base::processor::get().resolve("www.google.com", 80, ec);

    if(ec){
        cout << "Failed to resolve address!" << endl;
        return -1;
    }

    http::RAIIConnection connection{endpoint};

    if(!connection){
        cout << "Connection invalid!" << endl;
        return -1;
    }

    boost::beast::http::request<boost::beast::http::string_body> req;
    req.version(11); // HTTP 1.1
    req.method(boost::beast::http::verb::get); // GET
    req.target("/");
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send to host
    if(http::send(connection, req)){
        cout << "http::send fail!" << endl;
        return -1;
    }

    boost::beast::http::response<boost::beast::http::string_body> res;
    // Receive from host
    if(http::recv(connection, res)){
        cout << "http::recv fail!" << endl;
        return -1;
    }

    cout << res << endl;

    return 0;
}
