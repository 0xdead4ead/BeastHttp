#define BOOST_TEST_MODULE chain_router_test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <http/base/cb.hxx>
#include <http/base/regex.hxx>
#include <http/base/request_processor.hxx>

#include <http/literals.hxx>
#include <http/chain_router.hxx>

#include <boost/beast/http.hpp>

#include <unordered_map>

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

static const std::regex::flag_type regex_flags = std::regex::ECMAScript;

BOOST_AUTO_TEST_CASE(no_1) {

    http::chain_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.route("/testpath").get(
                [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/testpath");
    });

    router.route("/path/to/resource").get(
                [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/path/to/resource");
    });

    procs.provide({boost::beast::http::verb::get, "/testpath", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/path/to/resource", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(no_1)

BOOST_AUTO_TEST_CASE(no_2) {

    http::chain_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.route("/a/b/c").get(
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        std::next(_2x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/c");
    });

    router.route("/a/b/c/d")
      .get([](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d");
    }).post([](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d");
        BOOST_CHECK_EQUAL(request.body(), "postdata");
    }).put([](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d");
        BOOST_CHECK_EQUAL(request.body(), "putdata");
    });

    router.route("/a/b/c/d/e")
      .get([](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
    }).post([](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(request.body(), "postdata");
    }).put([](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "putdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(request.body(), "putdata");
    });

    procs.provide({boost::beast::http::verb::get, "/a/b/c", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d/e", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d/e", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d/e", 11, "putdata"}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(no_2)

BOOST_AUTO_TEST_CASE(literals_no_1) {

    using http::literals::operator""_route;

    http::chain_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    "^/a/b/c$"_route.advance(router).get(
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        std::next(_2x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/c");
    });

    "^/a/b/c/d$"_route.advance(router)
      .get([](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d");
    }).post([](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d");
        BOOST_CHECK_EQUAL(request.body(), "postdata");
    }).put([](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d");
        BOOST_CHECK_EQUAL(request.body(), "putdata");
    });

    "^/a/b/c/d/e$"_route.advance(router)
      .get([](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
    }).post([](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(request.body(), "postdata");
    }).put([](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "putdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(request.body(), "putdata");
    });

    procs.provide({boost::beast::http::verb::get, "/a/b/c", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d/e", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d/e", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d/e", 11, "putdata"}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(literals_no_1)
