# SYNOPSIS [![Build Status](https://travis-ci.org/0xdead4ead/BeastHttp.svg?branch=master)](https://travis-ci.org/0xdead4ead/BeastHttp)

Easy HTTP library implemented using C++14 and Boost.Beast. Allows you to get or provide REST resources available from an application in C ++. Use all the features of the Boost.Beast when constructing queries and answers.

## Work in progress...

# FEATURES

* Header-only
* HTTP 1.0 / 1.1
* TLS/SSL
* Pipeline
* Asynchronous/Synchronous request, response handling
* Timer manage (default action: Closing connection)
* Simple way to dynamic add REST resources using regex for path, and anonymous functions

# DEPENDENCIES

* Boost Libraries >= 1.66.0v
* [`Boost.Regex`](https://github.com/boostorg/regex), [`Boost.Asio`](https://github.com/boostorg/asio), [`Boost.Beast`](https://github.com/boostorg/beast/tree/develop) & [`Boost.LexicalCast`](https://github.com/boostorg/lexical_cast) library
* [`OpenSSL`](https://github.com/openssl/openssl) (optional)
* Minimal C++ 14 compatible compilator

# USAGE

More examples is [`here...`](https://github.com/0xdead4ead/BeastHttp/tree/dev/examples)
The following notes show the standard syntax for setting up routes and starting a server!

Alias's represent instances of default classes for reactive design.

```cpp

    using session_type = http::reactor::_default::session_type;
    using listener_type = http::reactor::_default::listener_type;

```

Add new route for GET request with "/" and "/a/b" resource: 

```cpp

    http::basic_router<session_type> router;

    router.get("^/$", [](auto request, auto context) {
        context.get().send(make_response(request, "MAIN PAGE\n"));
    });
    
    // and

    router.get("^/a/b$",
       [](auto /*request*/, auto /*context*/, auto _1x){
        // process /a
        std::next(_1x)();
    }, [](auto /*request*/, auto /*context*/){
        // process /b
    });

```

Add a route using a literal operator (as above)

```cpp

    using http::literals::operator""_get;

    "^/$"_get.advance(router, [](auto request, auto context) {
        context.get().send(make_response(request, "MAIN PAGE\n"));
    });
    
    // and
    
    "^/a/b$"_get.advance(router, 
       [](auto /*request*/, auto /*context*/, auto _1x){
        // process /a
        std::next(_1x)();
    }, [](auto /*request*/, auto /*context*/){
        // process /b
    });

```

Getting a parameter from a URI `/user/param?y=1992` :

```cpp

    router.param<int>(boost::regex::ECMAScript)
            .get("^/user/param[?]y=(\\d+)$", 
       [](auto /*request*/, auto /*context*/, auto args){
        assert(std::get<0>(args) == 1992);
    });

    // or

    router.param<int>(boost::regex::ECMAScript)
            .get("^/user/param[?]y=(\\d+)$",
       [](auto /*request*/, auto /*context*/, auto _1x, auto /*args*/){
        // process /user
        std::next(_1x)();
    }, [](auto /*request*/, auto /*context*/, auto args){
        // process /param
        assert(std::get<0>(args) == 1992);
    });

```

Getting a parameter using a string literal (as above) :

```cpp

    using http::literals::value;
    using http::literals::operator""_c;

    auto param = router.param<int>(boost::regex::ECMAScript);
    
    "^/user/param[?]y=(\\d+)$"_get.advance(std::move(param), 
       [](auto /*request*/, auto /*context*/, auto args){
        assert(value(args, 0_c) == 1992);
    });

    // or

    "^/user/param[?]y=(\\d+)$"_get.advance(std::move(param), 
       [](auto /*request*/, auto /*context*/, auto _1x, auto /*args*/){
        // process /user
        std::next(_1x)();
    }, [](auto /*request*/, auto /*context*/, auto args){
        // process /param
        assert(value(args, 0_c) == 1992);
    });

```

Create modular, mounted route handlers:

```cpp

    http::basic_router<session_type> animals;

    animals.get("^/cat$", [](auto request, auto context){ // '/animals/cat'
        context.get().send(make_response(request, "me-ow\n"));
    });

    animals.get("^/dog$", [](auto request, auto context){ // '/animals/dog'
        context.get().send(make_response(request, "aw! aw! Rrrrr\n"));
    });

    animals.get("^/mouse$", [](auto request, auto context){ // '/animals/mouse'
        context.get().send(make_response(request, "...\n"));
    });

    animals.get("^[/]??$", [](auto request, auto context){ // '/animals' or '/animals/'
        context.get().send(make_response(request, "animals home page\n"));
    });

    router.use("^/animals$", animals);

```

Create handlers routes, forming a chain, for the route path:

```cpp

    http::chain_router<session_type> books;

    books.route("^/book$")
            .get([](auto request, auto context) {
        context.get().send(make_response(request, "get a random book\n"));
    })
            .post([](auto request, auto context) {
        context.get().send(make_response(request, "add a book\n"));
    })
            .put([](auto request, auto context) {
        context.get().send(make_response(request, "update the book\n"));
    });

    router.use("^/books$", books);

```

Start listening on 127.0.0.1:80

```cpp

    // global namespace
    static boost::asio::io_context ioc;
    static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDOUT_FILENO)};
    //
    
    const auto& onError = [](auto code, auto from){
        http::out::pushn<std::ostream>(
                    out, "From:", from, "Info:", code.message());
    };

    const auto& onAccept = [&](auto socket){
        http::out::pushn<std::ostream>(
                    out, socket.remote_endpoint().address().to_string(), "connected!");

        session_type::recv(std::move(socket), router.resource_map(),
                           router.method_map(), boost::regex::ECMAScript, onError);
    };
    
    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned short>(80);

    listener_type::loop(ioc, {address, port}, onAccept, onError);

```

Run the I/O service on the requested number of threads:

```cpp

    std::thread t{[](){
        ioc.run();
    }};
    
    t.join();

```

# LICENSE

Copyright © 2018-2019 0xdead4ead

[BSD 2-Clause "Simplified" License](https://github.com/0xdead4ead/beast_http_server/blob/dev/LICENSE)
