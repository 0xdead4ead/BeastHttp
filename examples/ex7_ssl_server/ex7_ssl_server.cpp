#include <iostream>

#include <server.hpp>
#include <ssl.hpp>

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
//g++ -c -std=gnu++14 -I../../include -o ex7_ssl_server.o ./ex7_ssl_server.cpp
//g++ -o ex7_ssl_server ex7_ssl_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n -lssl

    //root@x0x0:~# curl --insecure https://localhost --request 'GET' --request-target '/1' --cacert /path/to/int.pem --cert-type PEM --tlsv1.2
    //root@x0x0:~# curl --insecure https://localhost --request 'GET' --request-target '/2' --cacert /path/to/int.pem --cert-type PEM --tlsv1.2
    //root@x0x0:~# openssl s_client -connect localhost:443 -servername localhost -CAfile /root/certs/int.pem

    std::string const cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDwTCCAimgAwIBAgIBATANBgkqhkiG9w0BAQsFADBAMQswCQYDVQQGEwJVUzEL\n"
    "MAkGA1UECAwCQ0ExDjAMBgNVBAoMBUJlYXN0MRQwEgYDVQQHDAtMb3MgQW5nZWxl\n"
    "czAeFw0xODA5MDEwOTUzMDJaFw0yMTA5MDEwOTUzMDJaMBQxEjAQBgNVBAMMCWxv\n"
    "Y2FsaG9zdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMm2gFNSV49z\n"
    "mwSn6qJeh8ABnLy61jBs/jmR53cN0cN0+8vBHrhQum4sCAsFgbiMZ4cJzE4d+g0p\n"
    "Y5qnF8N0jeqxGHg7d0YemTCJjvV9PN0esM02Fqd8sS473SEZpkoT5L4RDWaqggVN\n"
    "wfFnPeIVNIEE/QshygZyBNBJPqvzwM/buXR17ncQ8kOx0i5VLwrBKowVbG6mBIuF\n"
    "O96Vst+/mb0c4Lkrsev7hbjM7MFVjpCDm2zOLTYb4GVGHmqB1KPPPxzPiS+upvG1\n"
    "C5UstgJAbgfynIzRfriGTWPjLDoVhuq5aksJDGCjv14ini2LDJrqQ/AVVw0ZF/uh\n"
    "VbK/aS+ldD8CAwEAAaNyMHAwCQYDVR0TBAIwADAUBgNVHREEDTALgglsb2NhbGhv\n"
    "c3QwEwYDVR0lBAwwCgYIKwYBBQUHAwEwOAYIKwYBBQUHAQEELDAqMCgGCCsGAQUF\n"
    "BzABhhxodHRwOi8vZXhhbXBsZS5sb2NhbGhvc3Q6ODAvMA0GCSqGSIb3DQEBCwUA\n"
    "A4IBgQAkJbfWAkn7G3JZ3h1x3UzEdKOage9KmimbTW68HrVwyqsE057Ix0L3UVCd\n"
    "9T1xIeM9SFcsBTpA2mJotWhQak0tGlW9AcJoziRT1c4GvdkDwss0vAPB6XoCSZ9z\n"
    "bxyFdQaXRNK25kq60wSq1PTvNMZYYQA7Eusj5lpp1Gz+iS57NBfcq/MxiPB79Ysb\n"
    "6h+YkCPsJNx1S2W3qC2d3pIeOg+5lnXL58cj1XPnBgy84webRgPtxufKlVdfG85Z\n"
    "cw8a/OeXiCawZQKW5z7DwINsXEtX5cm4hMOlIE9JxaGCUf1yRel/MCT5fKaeSlUt\n"
    "4IeGaJvyC5zYiockngaJcCW2H2DieWkgRojfgGCagXQ3rs3bdKncNDg5iuu/7jXc\n"
    "TZ4YMoYmt78Z7D+Rjl624omUV2TYp3dU0xrG5Xutab3gJOrUzIn7/vtU+oJ3Kc7a\n"
    "Rk544OYp0lFUCgCuWsF9l2nDRcD5QQCDUveww9zQFXgkcGnJ4567Kcq+FlmS7fNo\n"
    "kNeiKJA=\n"
    "-----END CERTIFICATE-----\n";


    std::string const key =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDJtoBTUlePc5sE\n"
    "p+qiXofAAZy8utYwbP45ked3DdHDdPvLwR64ULpuLAgLBYG4jGeHCcxOHfoNKWOa\n"
    "pxfDdI3qsRh4O3dGHpkwiY71fTzdHrDNNhanfLEuO90hGaZKE+S+EQ1mqoIFTcHx\n"
    "Zz3iFTSBBP0LIcoGcgTQST6r88DP27l0de53EPJDsdIuVS8KwSqMFWxupgSLhTve\n"
    "lbLfv5m9HOC5K7Hr+4W4zOzBVY6Qg5tszi02G+BlRh5qgdSjzz8cz4kvrqbxtQuV\n"
    "LLYCQG4H8pyM0X64hk1j4yw6FYbquWpLCQxgo79eIp4tiwya6kPwFVcNGRf7oVWy\n"
    "v2kvpXQ/AgMBAAECggEAFW/s4W4N2jQKFIuX9xuex7wkITamDs12kz27YL66PVCQ\n"
    "dg4XKrWBqrXJQQqbx7Y8RnC3ItIywfVVHZmGgAJCFuAtXpHLUktsMmlcJSDjOAjL\n"
    "93M5IyGwXt6D2MG2F4dXtw9u4ita2B90bihvvjhMtS2HiwhTRS4W7t/p5jJomm5q\n"
    "RdWBGv6wqA6qHAMwyp/FoRY7gO4ZNbfCMn+n02A4PQ4fWZ/wIJgg9Ikl5cjinRon\n"
    "go6vbFakVr1CEpUJJyNMzSO0oNoOa0SPE90STRxdf+WlrCDjU84NNzjehGxVK2Nm\n"
    "KCyYtdaY1pSz2YQ0WczcbxFhYvzMLaRbnceMUY228QKBgQDtLLJ3Q44P1/TI9wSH\n"
    "BBiZPamSbwZmw40zYTNHDBQKcyGP07Anw9UiHWJcvWnMSRE/CtGjn4lw02azjukE\n"
    "Lx0mKUPdiodOGpr/qzw99lyM7Test8T/9cUR/g/p+lIRc9R1YR+ju6KU90Afv3yL\n"
    "z+Dy8K3kfeWmKCnumgxPEazDhwKBgQDZuTwsBLq6eiviMaTL1Rd8SZyHztPETq2I\n"
    "knzDRC0ZZ6qpAgfhUhQHLho8i/W93nIUwekO0y3P2ryBwx0t9GdEH2A7/8RRjQEK\n"
    "UVztb5Ugki3i3apqX3cPK9KIg6foTEYw4IxZTTjJxBN5BNMTJvQQM2tmfXppr94f\n"
    "v+SbkK7niQKBgQCgNPoUX7idcSXzfhA714N6N9HMjVyIm/1MQJMvobQD3wNDsR2j\n"
    "rr/QfILN3FCT4qNYr0kuunxPjy0nixhRcDXDakpiYsnE82nR2+wker7HnxFlhPj4\n"
    "YR6Oacx8I0++ZDyWUVXa9sr6zw0spN9PXcs4r2T3HCe9FhJFDx/TZUALDwKBgQCU\n"
    "9jZkC4xSX5o8tSiCSTY7VAXjqS+cRRRXt5ni43dTxWivH3OSxtxrGTDcMgodMN+u\n"
    "sgkpmnTinE6THZKOSYSJyEnIYzLHdQi8LXS+ArTuRvVcHbsl8lD8MUhnHGS5+82e\n"
    "TVPZGYt8CEomZ5Weqe0cVIHr6nfhbXE1Gc5oXTI9uQKBgFKbG+08h/UmEhBs78PT\n"
    "zVz15vXCD6CCZ/gGBxpDO9SJpVWJgo4m4MmmMn2zQyCJU/7vj4lp6oNsRA3ULdRL\n"
    "RbF5vQoY/3bZcyuKc2PfBAUjvKbLAAFF8VtVj6QUj0IgBKkkqumyvVxwYy/1k56R\n"
    "mXLnbU9LRnjes0GyZNw2gRBf\n"
    "-----END PRIVATE KEY-----\n";


    std::string const dh =
    "-----BEGIN DH PARAMETERS-----\n"
    "MIIBCAKCAQEAw5V8Zv0UXTzjBLBr+Wje5RktwL1K27giAQoZIKfs5MsKqAkaGJOI\n"
    "jeThplBGu26wZOxUKa0+aSU780JQY75aOYXqw6trLPC8Ay9ogQP9XzbxyJQPj2lJ\n"
    "LBwHnDVwU9xIYmwVBzo5QbVyssxtQlh+XckOARTQ4dz3x5lob9/W0Q6beRWZG7w6\n"
    "ruYU2DlZ5HMT2bMJkYV+T1Z6ZBVg8uXjuAvsqjHRJNDvKDPXWeZqHE4I4xFQo5MU\n"
    "ua0cgFeqJ9lzwiGKgTnwAswKA/c/XIX/xsCAdL1wp+a+U98loQfS/ZvWTg4Wer88\n"
    "18rx/G5U0pwJzRDqNbX2cwl3+3rj8KlsKwIBAg==\n"
    "-----END DH PARAMETERS-----\n";

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(boost::asio::buffer(cert.data(), cert.size()));
    ctx.use_private_key(boost::asio::buffer(key.data(), key.size()),
                        boost::asio::ssl::context::file_format::pem);
    ctx.use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));

//###############################################################################

    http::ssl::server my_https_server{ctx};

    using http::ssl::literal::operator""_get;
    using http::ssl::literal::operator""_all;

    R"(/1)"_get.assign(my_https_server, [](auto & req, auto & session){
        cout << req << endl; // '/1'
        session.do_write(make_response(req, "GET 1\n"));
     });

    R"(/2)"_get.assign(my_https_server, [](auto & req, auto & session){
        cout << req << endl; // '/2'
        session.do_write(make_response(req, "GET 2\n"));
     });

    R"(.*)"_all.assign(my_https_server, [](auto & req, auto & session){
        cout << req << endl; // 'any'
        session.do_write(make_response(req, "error\n"));
     });

    const auto & address = "127.0.0.1";
    uint32_t port = 443;

    my_https_server.listen(address, port, [](auto & session){
        http::base::out(session.getConnection()->stream().lowest_layer().remote_endpoint().address().to_string() + " connected");
        session.do_handshake();
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

    return 0;
}
