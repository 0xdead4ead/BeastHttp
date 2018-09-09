# SYNOPSIS [![Build Status](https://travis-ci.org/0xdead4ead/beast_http_server.svg?branch=master)](https://travis-ci.org/0xdead4ead/beast_http_server)

Easy HTTP library implemented using C++14 and Boost.Beast. Allows you to get or provide REST resources available from an application in C ++. Use all the features of the Boost.Beast when constructing queries and answers.

# FEATURES

* Header-only
* HTTP 1.0 / 1.1
* TLS/SSL
* Pipeline
* Asynchronous request, response handling
* Thread pool support
* Platform independent
* Simple way to add REST resources using regex for path, and anonymous functions

# DEPENDENCIES

* Boost Libraries >= 1.66.0v
* [`Boost.Regex`](https://github.com/boostorg/regex), [`Boost.Asio`](https://github.com/boostorg/asio), [`Boost.Beast`](https://github.com/boostorg/beast/tree/develop) & [`Boost.LexicalCast`](https://github.com/boostorg/lexical_cast) library
* [`JSON for Modern C++`](https://github.com/nlohmann/json) (optional)
* [`OpenSSL`](https://github.com/openssl/openssl) (optional)
* Minimal C++ 14 compatible compilator (for generic lambda)

# USAGE

More examples, an also JSON-RPC 2.0 server is [`here...`](https://github.com/0xdead4ead/beast_http_server/tree/dev/examples)

Add new route for GET request with "/" resource: 

```cpp

    http::server my_http_server;

    my_http_server.get("/", [](auto & req, auto & session){
        cout << req << endl;
    });

    // or

    my_http_server.get("/a/b", [](auto & req, auto & session, auto & next){
        // process '/a'
        next();
    }, [](auto & req, auto & session){
        // process '/b'
    });

```

Getting a parameter from a URI `/user/1992` :

```cpp

    my_http_server.param<int>().get("/user/(\\d+)",
       [](auto & req, auto & session, auto & args){
        assert(args._1 == 1992);
    });

    // or

    my_http_server.param<int>().get("/user/(\\d+)",
       [](auto & req, auto & session, auto & next, auto & args){
        // process '/user'
        next();
    }, [](auto & req, auto & session, auto & args){
        // process '/id'
        assert(args._1 == 1992);
    });

```

Create modular, mounted route handlers:

```cpp

    auto animals_router = my_http_server.BasicRouter();

    animals_router.get("/cat", [](auto & req, auto & session){ // '/animals/cat'
        session.do_write(make_response(req, "me-ow\n"));
    });

    animals_router.get("/dog", [](auto & req, auto & session){ // '/animals/dog'
        session.do_write(make_response(req, "aw! aw! Rrrrr\n"));
    });

    animals_router.get("/mouse", [](auto & req, auto & session){ // '/animals/mouse'
        session.do_write(make_response(req, "...\n"));
    });

    animals_router.get("[/]??", [](auto & req, auto & session){ // '/animals' or '/animals/'
        session.do_write(make_response(req, "animals home page\n"));
    });

    my_http_server.use("/animals", animals_router);

```

Create handlers routes, forming a chain, for the route path:

```cpp

    auto books_router = my_http_server.ChainRouter();

    books_router.route("/book") // 'books/book'
      .get([](auto & req, auto & session){
        session.do_write(make_response(req, "get a random book\n"));
    }).post([](auto & req, auto & session){
        session.do_write(make_response(req, "add a book\n"));
    }).put([](auto & req, auto & session){
        session.do_write(make_response(req, "update the book\n"));
    });

    my_http_server.use("/books", books_router);

```

Start listening on localhost:80

```cpp

    my_http_server.listen("127.0.0.1", 80, [](auto & session){
        http::base::out("New client!!!");
        session.do_read();
    });

```

Run the I/O service on the requested number of threads:

```cpp

    uint32_t pool_size = boost::thread::hardware_concurrency() * 2;
    http::base::processor::get().start( pool_size > 0 ? pool_size :  4 );
    http::base::processor::get().wait();

```

Request content from the server:

```cpp

    http::client my_http_client;

    const auto & host = "www.example.com";
    auto port = 80;

    const auto & on_connect = [&host](auto & session){
        http::base::out("Successful connected!"); 
        session.do_write(make_request());
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

Copyright © 2018 0xdead4ead
