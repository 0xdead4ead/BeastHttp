# SYNOPSIS

Easy HTTP client library implemented using C++11 and Boost.Beast. Allows you to get or provide REST resources available from an application in C ++. Use all the features of the [`Boost.Beast`](https://www.boost.org/doc/libs/1_67_0/libs/beast/doc/html/index.html) when constructing queries and answers.

# FEATURES

* Header-only
* HTTP 1.0 / 1.1 / 2.0
* Asynchronous request, response handling
* Thread pool support
* Platform independent
* Simple way to add REST resources using regex for path, and anonymous functions

# DEPENDENCIES

* Boost.Asio & Boost.Beast library
* C++ 14 compatible compilator (for generic lambda)

# USAGE

Server and client examples -> `./examples`

```cpp

    http::server my_http_server;

    // Add handler on GET request with "/" resource
    my_http_server.get("^/$", http::method_type::get, [](auto & req, auto & session){

        cout << *req << endl;

    });

    // Start listening on localhost
    my_http_server.listen("127.0.0.1", 80, [](auto & session){

        http::base::out("New client!!!");
        session->do_read();
    });

    // Run the I/O service on the requested number of threads
    uint32_t pool_size = boost::thread::hardware_concurrency();
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

```