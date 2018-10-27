# SYNOPSIS [![Build Status](https://travis-ci.org/0xdead4ead/BeastHttp.svg?branch=master)](https://travis-ci.org/0xdead4ead/BeastHttp)

Easy HTTP library implemented using C++14 and Boost.Beast. Allows you to get or provide REST resources available from an application in C ++. Use all the features of the Boost.Beast when constructing queries and answers. The implementations websocket is [`here...`](https://github.com/0xdead4ead/beast_ws_server)

# FEATURES

* Header-only
* HTTP 1.0 / 1.1
* TLS/SSL
* Pipeline
* Asynchronous/Synchronous request, response handling
* Thread pool support
* Timer manage (default timeout: 10 seconds, default action: Closing connection)
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

    http::server instance;

    instance.get("/", 
       [](auto & req, auto & /*session*/){
        cout << req << endl;
    });

    // or

    instance.get("/a/b", 
       [](auto & /*req*/, auto & /*session*/, auto & next){
        // process '/a'
        next();
    }, [](auto & /*req*/, auto & /*session*/){
        // process '/b'
    });

```

Add a route using a literal operator (as above)

```cpp

    "/"_get.assign(instance, 
       [](auto & req, auto & /*session*/){
        cout << req << endl;
    });
    
    // or
    
    "/a/b"_get.assign(instance, 
       [](auto & /*req*/, auto & /*session*/, auto & next){
        // process '/a'
        next();
    }, [](auto & /*req*/, auto & /*session*/){
        // process '/b'
    })

```

Getting a parameter from a URI `/user/1992` :

```cpp

    instance.param<int>().get("/user/(\\d+)",
       [](auto & /*req*/, auto & /*session*/, auto & args){
        assert(args._1 == 1992);
    });

    // or

    instance.param<int>().get("/user/(\\d+)",
       [](auto & /*req*/, auto & /*session*/, auto & next, auto & /*args*/){
        // process '/user'
        next();
    }, [](auto & /*req*/, auto & /*session*/, auto & args){
        // process '/id'
        assert(args._1 == 1992);
    });

```

Getting a parameter using a string literal (as above) :

```cpp

    "/user/(\\d+)"_get.assign(instance.param<int>())
       .with([](auto & /*req*/, auto & /*session*/, auto & args){
        assert(args._1 == 1992);
    });

    // or

    "/user/(\\d+)"_get.assign(instance.param<int>())
       .with([](auto & /*req*/, auto & /*session*/, auto & next, auto & /*args*/){
        // process '/user'
        next();
    }, [](auto & /*req*/, auto & /*session*/, auto & args){
        // process '/id'
        assert(args._1 == 1992);
    });

```

Create modular, mounted route handlers:

```cpp

    auto animals_router = instance.BasicRouter();

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

    instance.use("/animals", animals_router);

```

Create handlers routes, forming a chain, for the route path:

```cpp

    auto books_router = instance.ChainRouter();

    books_router.route("/book") // 'books/book'
      .get([](auto & req, auto & session){
        session.do_write(make_response(req, "get a random book\n"));
    }).post([](auto & req, auto & session){
        session.do_write(make_response(req, "add a book\n"));
    }).put([](auto & req, auto & session){
        session.do_write(make_response(req, "update the book\n"));
    });

    instance.use("/books", books_router);

```

Start listening on localhost:80

```cpp

    instance.listen("127.0.0.1", 80, [](auto & session){
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

Request content from the server (Async):

```cpp

    http::client instance;

    instance.on_connect = [](auto & session){
        http::base::out("Successful connected!");
        session.do_write(make_request());
    };

    instance.on_message = [](auto & res, auto & session){
        cout << res << endl;
        
        // The answer is received, stop!
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

```

Request content from the server (Sync):

```cpp

    boost::system::error_code ec;
    auto connection_p = http::base::processor::get().create_connection
                                         <http::base::connection>("www.example.com", 80, ec);

    if(!connection_p){
        cout << "Failed to resolve address!" << endl;
        return -1;
    }

    if(ec){
        cout << "Connection invalid!" << endl;
        return -1;
    }
    
    // Send to host
    if(http::send_request(connection_p, make_request())){
        cout << "http::send_request fail!" << endl;
        return -1;
    }

    auto res = make_response();
    // Receive from host
    if(http::recv_responce(connection_p, res)){
        cout << "http::recv_responce fail!" << endl;
        return -1;
    }

    cout << res << endl;

```

# LICENSE

Copyright © 2018 0xdead4ead

[BSD 2-Clause "Simplified" License](https://github.com/0xdead4ead/beast_http_server/blob/dev/LICENSE)
