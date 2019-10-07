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
* Server-Sent Events
* Simple way to dynamic add REST resources using regex for path, and anonymous functions
* Support accepted plain and SSL session on the same port

# DEPENDENCIES

* Boost Libraries >= 1.70.0
* [`Boost.Asio`](https://github.com/boostorg/asio), [`Boost.Beast`](https://github.com/boostorg/beast/tree/develop)
* [`OpenSSL`](https://github.com/openssl/openssl) (optional)
* [`Boost.LexicalCast`](https://github.com/boostorg/lexical_cast) (optional)

# USAGE

More examples is [`here...`](https://github.com/0xdead4ead/BeastHttp/tree/dev/BeastHttp/src/examples)
The following notes show the standard syntax for setting up routes and starting a server!

Alias's represent instances of default classes for reactive (select/epool) design.

```cpp

    using namespace _0xdead4ead;
    namespace beast = boost::beast;

    using http_session = http::reactor::_default::session_type;
    using http_listener = http::reactor::_default::listener_type;

```

Creating a new callback functions storage and route path for GET request with "/" resource:

```cpp

    static const std::regex::flag_type regex_flags = std::regex::ECMAScript;

    http::basic_router<http_session> router{regex_flags};

    router.get("^/$", [](auto beast_http_request, auto context) {
        context.send(make_200<beast::http::string_body>(beast_http_request, "Main page\n", "text/html"));
    });

    router.all("^.*$", [](auto beast_http_request, auto context) {
        context.send(make_404<beast::http::string_body>(beast_http_request, "Resource is not found\n", "text/html"));
    });

    // or so ...

    using http::literals::operator""_get;

    "^/$"_get.advance(router, [](auto beast_http_request, auto context) {
        // as above...
    });

    "^.*$"_all.advance(router, [](auto beast_http_request, auto context) {
        // as above...
    });

```

It is possible to add multiple handlers for one route. For movement on them `std::next` or `std::advance` is used:

```cpp
    router.get("^/a/b$",
       [](auto /*beast_http_request*/, auto /*context*/, auto iterator){
        // process /a
        std::next(iterator)();
    }, [](auto /*beast_http_request*/, auto /*context*/){
        // process /b
    });

```

Getting a parameter from a URI. Request send example `curl localhost --request 'GET' --request-target '/user/param?y=1992'`:

```cpp

    using pack = http::param::pack<int>;

    router.param<pack>().get("^/user/param[?]y=(\\d+)$",
       [](auto /*beast_http_request*/, auto /*context*/, auto args){
        assert(std::get<0>(args) == 1992);
    });

    // or

    router.param<pack>().get("^/user/param[?]y=(\\d+)$",
       [](auto /*beast_http_request*/, auto /*context*/, auto iterator, auto /*args*/){
        // process /user
        std::next(iterator)();
    }, [](auto /*beast_http_request*/, auto /*context*/, auto args){
        // process /param
        assert(std::get<0>(args) == 1992);
    });

```

Getting a parameter using a string literal (as above) :

```cpp

    // For value f'n is required c++14
    using http::literals::value;
    using http::literals::operator""_c;

    auto param = router.param<pack>();

    "^/user/param[?]y=(\\d+)$"_get.advance(std::move(param),
       [](auto /*beast_http_request*/, auto /*context*/, auto args){
        assert(value(args, 0_c) == 1992);
    });

```

Create modular, mounted route handlers:

```cpp

    http::basic_router<http_session> animals{regex_flags};

    animals.get("^/cat$", [](auto beast_http_request, auto context){ // '/animals/cat'
        context.send(make_200<beast::http::string_body>(beast_http_request, "me-ow\n", "text/html"));
    });

    animals.get("^/dog$", [](auto beast_http_request, auto context){ // '/animals/dog'
        context.send(make_200<beast::http::string_body>(beast_http_request, "aw! aw! Rrrrr\n", "text/html"));
    });

    animals.get("^/mouse$", [](auto beast_http_request, auto context){ // '/animals/mouse'
        context.send(make_200<beast::http::string_body>(beast_http_request, "...\n", "text/html"));
    });

    animals.get("^[/]??$", [](auto beast_http_request, auto context){ // '/animals' or '/animals/'
        context.send(make_200<beast::http::string_body>(beast_http_request, "animals home page\n", "text/html"));
    });

    router.use("^/animals$", animals);

```

Create handlers routes, forming a chain, for the route path:

```cpp

    http::chain_router<http_session> books{regex_flags};

    books.route("^/book$")
            .get([](auto beast_http_request, auto context) {
        context.send(make_200<beast::http::string_body>(beast_http_request, "get a random book\n", "text/html"));
    })
            .post([](auto beast_http_request, auto context) {
        context.send(make_200<beast::http::string_body>(beast_http_request, "add a book\n", "text/html"));
    })
            .put([](auto beast_http_request, auto context) {
        context.send(make_200<beast::http::string_body>(beast_http_request, "update the book\n", "text/html"));
    });

    router.use("^/books$", books);

```

Start listening on 0.0.0.0:8080

```cpp

    // global namespace
    static boost::asio::io_context ioc;
    static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDERR_FILENO)};
    //

    const auto& onError = [](auto system_error_code, auto from){
        http::out::pushn<std::ostream>(
                    out, "From:", from, "Info:", system_error_code.message());
    };

    const auto& onAccept = [&](auto asio_socket){
        auto endpoint = asio_socket.remote_endpoint();

        http::out::prefix::version::time::pushn<std::ostream>(
                    out, endpoint.address().to_string() + ':' + std::to_string(endpoint.port()), "connected!");

        http_session::recv(std::move(asio_socket), router, onError);
    };

    auto const address = boost::asio::ip::address_v4::any();
    auto const port = static_cast<unsigned short>(8080);

    http_listener::launch(ioc, {address, port}, onAccept, onError);

```

Run the I/O service on the requested number of threads:

```cpp

    std::thread t{[](){
        ioc.run();
    }};

    // do other work...

    t.join();

```
* Copyright (c) [@Evgeny Tixonow](https://github.com/0xdead4ead) 2018-2019
