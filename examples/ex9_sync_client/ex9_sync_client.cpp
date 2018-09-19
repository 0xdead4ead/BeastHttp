#include <iostream>

#include <client.hpp>

using namespace std;

int main()
{

    auto connection_p = http::base::processor::get().create_connection<http::base::connection>("www.google.com", 80);

    boost::beast::http::request<boost::beast::http::string_body> req;
    req.version(11); // HTTP 1.1
    req.method(boost::beast::http::verb::get); // GET
    req.target("/");
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send to host
    if(http::send_request(connection_p, req)){
        cout << "http::send_request fail!" << endl;
        return -1;
    }

    boost::beast::http::response<boost::beast::http::string_body> res;
    // Receive from host
    if(http::recv_responce(connection_p, res)){
        cout << "http::recv_responce fail!" << endl;
        return -1;
    }

    cout << res << endl;

    connection_p->close(); // auto call with destroying

    return 0;
}
