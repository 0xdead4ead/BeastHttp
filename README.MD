# SYNOPSIS

Easy HTTP library implemented using C++14 and Boost.Beast. Allows you to get or provide REST resources available from an application in C ++. Use all the features of the Boost.Beast when constructing queries and answers.

> **base.hpp:** Add/remove listener, create connections, manage signals, io, ...

> **session.hpp:** Handles an HTTP server/client connection

> **client.hpp:** Class for communication with a remote host

> **server.hpp:** HTTP server class, registration of new routes

> **list_cb.hpp:** Container for storing callback functions

# FEATURES

* Header-only
* HTTP 1.0 / 1.1 / 2.0
* Asynchronous request, response handling
* Thread pool support
* Platform independent
* Simple way to add REST resources using regex for path, and anonymous functions

# DEPENDENCIES

* [`Boost.Regex`](https://github.com/boostorg/regex), [`Boost.Asio`](https://github.com/boostorg/asio) & [`Boost.Beast`](https://github.com/boostorg/beast/tree/develop) library
* Minimal C++ 14 compatible compilator (for generic lambda)

# USAGE

Server and client examples -> `./examples`

Add new route for GET request with "/" resource 

```cpp

    http::server my_http_server;

    my_http_server.get("/", [](auto & req, auto & session){
        cout << req << endl;
    });

    // and chain handling

    my_http_server.get("/a/b", [](auto & req, auto & session, auto & next){
        // process a
        next();
    }, [](auto & req, auto & session){
        // process b
    });

```

Start listening on localhost:80

```cpp

    my_http_server.listen("127.0.0.1", 80, [](auto & session){
        http::base::out("New client!!!");
        session.do_read();
    });

```

Run the I/O service on the requested number of threads

```cpp

    uint32_t pool_size = boost::thread::hardware_concurrency() * 2;
    http::base::processor::get().start( pool_size > 0 ? pool_size :  4 );
    http::base::processor::get().wait();

```

Request content from the server

```cpp

    http::client my_http_client;

    const auto & host = "www.example.com";
    auto port = 80;

    const auto & on_connect = [&host](auto & session){
        http::base::out("Successful connected!");

        // Create request and fill fields...
        
        // Send 
        // session.do_write(boost::move(req));
    };

    // Handler on received object
    const auto & on_receive = [](auto & res, auto & session){
        cout << res << endl;

        session.do_close();
        // The answer is received, stop!
        http::base::processor::get().stop();
    };

    // Connect to host:port
    my_http_client.invoke(host, port, on_connect, on_receive);

```
