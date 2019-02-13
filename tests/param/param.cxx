#define BOOST_TEST_MODULE param_test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <base/cb.hxx>
#include <base/regex.hxx>
#include <base/request_processor.hxx>
#include <basic_router.hxx>
#include <param.hxx>

#include <boost/beast/http.hpp>

class test_session
{
public:

    using self_type = test_session;

    template<class>
    class context;

    class flesh;

    using flesh_type = flesh;

    using context_type = context<flesh_type>;

    using reference_wrapper = std::reference_wrapper<context_type const>;

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

BOOST_AUTO_TEST_CASE(no_1) {

    http::basic_router<test_session> router;

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), boost::regex::ECMAScript};

    router.param<http::param::pack<int>>(boost::regex::ECMAScript)
            .get("/(\\d+)", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
    });

    router.param<http::param::pack<int, int>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
    });

    router.param<http::param::pack<int, int, std::string>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
    });

    router.param<http::param::pack<int, int, std::string, int>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
    });

    router.param<http::param::pack<int, int, std::string, int, int>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
    });

    router.param<http::param::pack<int, int, std::string, int, int, std::string>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30/X");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
        BOOST_CHECK_EQUAL(std::get<5>(args), "X");
    });

    router.param<http::param::pack<int, int, std::string, int, int, std::string, unsigned int>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])/(\\d+)", [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10/20/abc/30/-30/Y/1000");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
        BOOST_CHECK_EQUAL(std::get<1>(args), 20);
        BOOST_CHECK_EQUAL(std::get<2>(args), "abc");
        BOOST_CHECK_EQUAL(std::get<3>(args), 30);
        BOOST_CHECK_EQUAL(std::get<4>(args), -30);
        BOOST_CHECK_EQUAL(std::get<5>(args), "Y");
        BOOST_CHECK_EQUAL(std::get<6>(args), 1000);
    });

    router.param<http::param::pack<int, int, std::string, int, int, std::string, uint16_t, float, char, char, char>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])/(\\d+)/(\\d+.\\d+)/(X)(Y)(Z)", [](auto request, auto /*context*/, auto args){
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

    router.param<http::param::pack<int, int, std::string, int, int, std::string, uint16_t, float, char, char, char, uint8_t>>(boost::regex::ECMAScript)
            .get("/(\\d+)/(\\d+)/(\\w+)/(\\d+)/(-\\d+)/([XYZ])/(\\d+)/(\\d+.\\d+)/(X)(Y)(Z)/(\\d)", [](auto request, auto /*context*/, auto args){
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

    router.param<http::param::pack<int, int, int>>(boost::regex::ECMAScript)
            .get("/param[?]o=(\\d+)&t=(\\d+)&f=(\\d+)", [](auto request, auto /*context*/, auto args){
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

    http::basic_router<test_session> router;

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), boost::regex::ECMAScript};

    router.param<http::param::pack<int>>(boost::regex::ECMAScript)
            .get("/a/(\\d+)",
       [](auto request, auto /*context*/, auto _1x, auto args){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(std::get<0>(args), 0);
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto args){
        BOOST_CHECK(request.target() == "/10");
        BOOST_CHECK_EQUAL(std::get<0>(args), 10);
    });

    router.param<http::param::pack<int, std::string>>(boost::regex::ECMAScript)
            .get("/a/(\\d+)/b/(\\w+)",
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

    router.param<http::param::pack<int, int, int>>(boost::regex::ECMAScript)
            .get("/a/(\\d+)+(\\d+)/b",
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

    router.param<http::param::pack<int, int, int>>(boost::regex::ECMAScript)
            .get("/a/(\\d+)/b/(\\d+)/c/(\\d+)",
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
