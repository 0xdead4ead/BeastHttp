#define BOOST_TEST_MODULE param_test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <http/base/cb.hxx>
#include <http/base/regex.hxx>
#include <http/base/request_processor.hxx>

#include <http/param.hxx>
#include <http/basic_router.hxx>
#include <http/chain_router.hxx>
#include <http/literals.hxx>

#include <boost/beast/http.hpp>

using namespace _0xdead4ead;

class test_session
{
public:

    using self_type = test_session;

    template<class>
    class context;

    class flesh;

    using flesh_type = flesh;

    using context_type = context<flesh_type>;

    using resource_regex_type = std::string;

    using resource_type = boost::beast::string_view;

    using method_type = boost::beast::http::verb;

    using body_type = boost::beast::http::string_body;

    using cbexecutor_type = http::base::cb::executor;

    using request_type = boost::beast::http::request<body_type>;

    using regex_type = http::base::regex;

    using regex_flag_type = typename regex_type::flag_type;

    using storage_type = http::base::cb::storage<self_type, std::function, std::vector>;

    using resource_map_type = std::unordered_map<resource_regex_type, std::shared_ptr<storage_type>>;

    using method_map_type = std::map<method_type, resource_map_type>;

    class flesh
    {};

    template<class Flesh>
    class context
    {
    public:

        context(Flesh&)
        {}
    };

}; // class test_session

static const boost::regex::flag_type regex_flags = boost::regex::ECMAScript;

BOOST_AUTO_TEST_CASE(no_1) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    using pack0 = http::param::pack<int>;

    router.param<pack0>().get("/(\\d+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
    });

    using pack1 = http::param::pack<int, int>;

    router.param<pack1>().get("/(\\d+)/(\\d+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
    });

    using pack2 = http::param::pack<int, int, std::string>;

    router.param<pack2>().get("/(\\d+)/(\\d+)/(\\w+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
    });

    using pack3 = http::param::pack<int, int, std::string, int>;

    router.param<pack3>().get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
    });

    using pack4 = http::param::pack<int, int, std::string, int, int>;

    router.param<pack4>().get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
    });

    using pack5 = http::param::pack<int, int, std::string, int, int, std::string>;

    router.param<pack5>().get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30/X");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
        BOOST_CHECK_EQUAL(std::get<5>(args), "X");
    });

    using pack6 = http::param::pack<int, int, std::string, int, int, std::string, unsigned int>;

    router.param<pack6>().get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])/(\\d+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30/Y/1000");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
        BOOST_CHECK_EQUAL(std::get<5>(args), "Y");
        BOOST_CHECK_EQUAL(std::get<6>(args), 1000);
    });

    using pack7 = http::param::pack<int, int, std::string, int, int, std::string, uint16_t, float, char, char, char>;

    router.param<pack7>().get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])/(\\d+)/(\\d+.\\d+)/(X)(Y)(Z)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30/Y/1000/12.3/XYZ");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
        BOOST_CHECK_EQUAL(std::get<5>(args), "Y");
        BOOST_CHECK_EQUAL(std::get<6>(args), 1000);
        BOOST_CHECK_EQUAL(static_cast<int>(std::get<7>(args)), 12);
        BOOST_CHECK_EQUAL(std::get<8>(args), 'X');
        BOOST_CHECK_EQUAL(std::get<9>(args), 'Y');
        BOOST_CHECK_EQUAL(std::get<10>(args), 'Z');
    });

    using pack8 = http::param::pack<int, int, std::string, int, int, std::string, uint16_t, float, char, char, char, uint8_t>;

    router.param<pack8>().get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])/(\\d+)/(\\d+.\\d+)/(X)(Y)(Z)/(\\d)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30/Y/1000/12.3/XYZ/0");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
        BOOST_CHECK_EQUAL(std::get<5>(args), "Y");
        BOOST_CHECK_EQUAL(std::get<6>(args), 1000);
        BOOST_CHECK_EQUAL(static_cast<int>(std::get<7>(args)), 12);
        BOOST_CHECK_EQUAL(std::get<8>(args), 'X');
        BOOST_CHECK_EQUAL(std::get<9>(args), 'Y');
        BOOST_CHECK_EQUAL(std::get<10>(args), 'Z');
        BOOST_CHECK_EQUAL(std::get<11>(args), 0x30);
    });

    using pack9 = http::param::pack<int, int, int>;

    router.param<pack9>().get("/param[?]o=(\\d+)&t=(\\d+)&f=(\\d+)",
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/param?o=10&t=20&f=30");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), 30);
    });

    procs.provide({boost::beast::http::verb::get, "/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30/-30", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30/-30/X", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30/-30/Y/1000", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30/-30/Y/1000/12.3", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30/-30/Y/1000/12.3/XYZ", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/10/20/abc/30/-30/Y/1000/12.3/XYZ/0", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/param?o=10&t=20&f=30", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(no_1)

BOOST_AUTO_TEST_CASE(no_2) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    using pack0 = http::param::pack<int>;

    router.param<pack0>().get("/a/(\\d+)",
       [](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(std::get<0>(args), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
    });

    using pack1 = http::param::pack<int, std::string>;

    router.param<pack1>().get("/a/(\\d+)/b/(\\w+)",
       [](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(std::get<0>(args), 0);
        BOOST_CHECK_EQUAL(std::get<1>(args), "");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x, auto args){
        BOOST_CHECK(request.target() == "/20");
        BOOST_CHECK_EQUAL(std::get<0>(args), 20);
        BOOST_CHECK_EQUAL(std::get<1>(args), "");
        std::next(_2x)();
    }, [](auto request, auto /*context*/, auto _3x, auto args){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(std::get<0>(args), 20);
        BOOST_CHECK_EQUAL(std::get<1>(args), "");
        std::next(_3x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/cde");
        BOOST_CHECK_EQUAL(std::get<0>(args), 20);
        BOOST_CHECK_EQUAL(std::get<1>(args), "cde");
    });

    using pack2 = http::param::pack<int, int, int>;

    router.param<pack2>().get("/a/(\\d+)+(\\d+)/b",
       [](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(std::get<0>(args), 0);
        BOOST_CHECK_EQUAL(std::get<1>(args), 0);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x, auto args){
        BOOST_CHECK(request.target() == "/1+2");
        BOOST_CHECK_EQUAL(std::get<0>(args), 1);
        BOOST_CHECK_EQUAL(std::get<1>(args), 2);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
        std::next(_2x)();
    }, [](auto request, auto /*context*/, auto _3x, auto args){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(std::get<0>(args), 1);
        BOOST_CHECK_EQUAL(std::get<1>(args), 2);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
        std::next(_3x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(std::get<0>(args), 1);
        BOOST_CHECK_EQUAL(std::get<1>(args), 2);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
    });

    router.param<pack2>().get("/a/(\\d+)/b/(\\d+)/c/(\\d+)",
       [](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(std::get<0>(args), 0);
        BOOST_CHECK_EQUAL(std::get<1>(args), 0);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
        std::next(_1x, 2)(); // Skip /1
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/, auto /*args*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x, auto args){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(std::get<0>(args), 0);
        BOOST_CHECK_EQUAL(std::get<1>(args), 0);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
        std::next(_3x)();
    }, [](auto request, auto /*context*/, auto _3x, auto args){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK_EQUAL(std::get<0>(args), 2);
        BOOST_CHECK_EQUAL(std::get<1>(args), 0);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
        std::next(_3x, 2)(); // Skip /c
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/, auto /*args*/){
        // Empty
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/3");
        BOOST_CHECK_EQUAL(std::get<0>(args), 2);
        BOOST_CHECK_EQUAL(std::get<1>(args), 3);
        BOOST_CHECK_EQUAL(std::get<2>(args), 0);
    });

    procs.provide({boost::beast::http::verb::get, "/a/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/20/b/cde", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/1+2/b", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/1/b/2/c/3", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(no_2)

BOOST_AUTO_TEST_CASE(literals_no_1) {

    using http::literals::value;
    using http::literals::operator""_c;
    using http::literals::operator""_get;

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    using pack0 = http::param::pack<int>;

    "^/(\\d+)$"_get.advance(router.param<pack0>(),
       [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    });

    "^/a/(\\d+)$"_get.advance(router.param<pack0>(),
       [](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(value(args, 0_c), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    });

    procs.provide({boost::beast::http::verb::get, "/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/10", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(literals_no_1)

BOOST_AUTO_TEST_CASE(literals_no_2) {

    using http::literals::value;
    using http::literals::operator""_c;
    using http::literals::operator""_route;

    http::chain_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    using pack0 = http::param::pack<int>;

    "^/(\\d+)$"_route.advance(router.param<pack0>())
      .get([](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    }).post([](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    }).put([](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    });

    "^/a/(\\d+)$"_route.advance(router.param<pack0>())
       .get([](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(value(args, 0_c), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    }).post([](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(value(args, 0_c), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    }).put([](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(value(args, 0_c), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(value(args, 0_c), 10);
    });

    procs.provide({boost::beast::http::verb::get, "/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/10", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/10", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(literals_no_2)
