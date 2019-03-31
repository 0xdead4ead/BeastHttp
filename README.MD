# SYNOPSIS [![license][badge.license]][license] [![build][badge.build]][build] ![badge.cpp](https://img.shields.io/badge/c++-11/14-ff69b4.svg?style=flat-square)

[badge.license]: http://img.shields.io/badge/license-BSD%E2%80%932%E2%80%93Clause-blue.svg?style=flat-square
[badge.build]: https://img.shields.io/travis/0xdead4ead/BeastHttp.svg?style=flat-square&logo=travis

[license]: https://github.com/0xdead4ead/BeastHttp/blob/master/LICENSE
[build]: https://travis-ci.org/0xdead4ead/BeastHttp

Easy HTTP (Header-only) library implemented using minimum C++11 and Boost.Beast. Allows you to get or provide REST resources available from an application in C ++. Use all the features of the Boost.Beast when constructing queries and answers.

# FEATURES

* HTTP 1.0 / 1.1
* TLS/SSL
* Pipeline request
* Used I/O multiplexing model (Asio library implementation)
* Timer manage (default action: Closing connection)
* Simple way to dynamic add REST resources using regex for path, and anonymous functions
* Support accepted plain and SSL session on the same port

# DEPENDENCIES

* Boost Libraries >= 1.66.0v
* [`Boost.Regex`](https://github.com/boostorg/regex), [`Boost.Asio`](https://github.com/boostorg/asio), [`Boost.Beast`](https://github.com/boostorg/beast/tree/develop) & [`Boost.LexicalCast`](https://github.com/boostorg/lexical_cast) library
* [`OpenSSL`](https://github.com/openssl/openssl) (optional)

# USAGE

More examples is [`here...`](https://github.com/0xdead4ead/BeastHttp/tree/dev/examples)
The following notes show the standard syntax for setting up routes and starting a server!

Alias's represent instances of default classes for reactive (select/epool) design.

```cpp

    using namespace _0xdead4ead;

    using HttpSession = http::reactor::_default::session_type;
    using HttpListener = http::reactor::_default::listener_type;

```

Creating a new callback functions storage and route path for GET request with "/" resource: 

```cpp

    http::basic_router<HttpSession> router;

    router.get("^/$", [](auto request, auto context) {
        context.send(make_response(request, "MAIN PAGE\n"));
    });
    
    // or so ...

    using http::literals::operator""_get;

    "^/$"_get.advance(router, [](auto request, auto context) {
        context.send(make_response(request, "MAIN PAGE\n"));
    });

```

It is possible to add multiple handlers for one route. For movement on them `std::next` or `std::advance` is used:

```cpp
    router.get("^/a/b$",
       [](auto /*request*/, auto /*context*/, auto _1x){
        // process /a
        std::next(_1x)();
    }, [](auto /*request*/, auto /*context*/){
        // process /b
    });

```

Getting a parameter from a URI. Request send example `curl localhost --request 'GET' --request-target '/user/param?y=1992'`:

```cpp

    using pack = http::param::pack<int>;

    router.param<pack>(boost::regex::ECMAScript)
            .get("^/user/param[?]y=(\\d+)$", 
       [](auto /*request*/, auto /*context*/, auto args){
        assert(std::get<0>(args) == 1992);
    });

    // or

    router.param<pack>(boost::regex::ECMAScript)
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

    // For value f'n is required c++14
    using http::literals::value;
    using http::literals::operator""_c;

    auto param = router.param<pack>(boost::regex::ECMAScript);
    
    "^/user/param[?]y=(\\d+)$"_get.advance(std::move(param), 
       [](auto /*request*/, auto /*context*/, auto args){
        assert(value(args, 0_c) == 1992);
    });

```

Create modular, mounted route handlers:

```cpp

    http::basic_router<HttpSession> animals;

    animals.get("^/cat$", [](auto request, auto context){ // '/animals/cat'
        context.send(make_response(request, "me-ow\n"));
    });

    animals.get("^/dog$", [](auto request, auto context){ // '/animals/dog'
        context.send(make_response(request, "aw! aw! Rrrrr\n"));
    });

    animals.get("^/mouse$", [](auto request, auto context){ // '/animals/mouse'
        context.send(make_response(request, "...\n"));
    });

    animals.get("^[/]??$", [](auto request, auto context){ // '/animals' or '/animals/'
        context.send(make_response(request, "animals home page\n"));
    });

    router.use("^/animals$", animals);

```

Create handlers routes, forming a chain, for the route path:

```cpp

    http::chain_router<HttpSession> books;

    books.route("^/book$")
            .get([](auto request, auto context) {
        context.send(make_response(request, "get a random book\n"));
    })
            .post([](auto request, auto context) {
        context.send(make_response(request, "add a book\n"));
    })
            .put([](auto request, auto context) {
        context.send(make_response(request, "update the book\n"));
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

    HttpListener::launch(ioc, {address, port}, onAccept, onError);
    
    // Note: 
    // After starting the listener instance, 
    // it is not recommended to add new handlers from other threads, 
    // except those that are in a loop io_context::run, to the router 
    // used for processing client requests.

```

Run the I/O service on the requested number of threads:

```cpp

    std::thread t{[](){
        ioc.run();
    }};
    
    t.join();

```
* Copyright (c) [@Evgeny Tixonow #0xdead4ead](https://github.com/0xdead4ead) 2018-2019
