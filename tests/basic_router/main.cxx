#define BOOST_TEST_MODULE basic_router_test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <base/cb.hxx>
#include <base/regex.hxx>
#include <base/request_processor.hxx>

#include <literals.hxx>
#include <basic_router.hxx>

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

BOOST_AUTO_TEST_CASE(get_no_1) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.get("/testpath", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/testpath");
    });

    router.get("/path/to/resource", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/path/to/resource");
    });

    procs.provide({boost::beast::http::verb::get, "/testpath", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/path/to/resource", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(get_no_1)

BOOST_AUTO_TEST_CASE(get_no_2) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.get("/a/b/c",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        std::next(_2x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/c");
    });

    router.get("/a/b/c/d",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x, 2)(); // Skip /b
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/c");
        std::next(_3x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/d");
    });

    router.get("/a/b/c/d/e",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        std::next(_2x, 3)(); // Skip /c, /d
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/e");
    });

    router.get("/a/b/c/d/e/f",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x, 2)(); // Skip /b
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/c");
        std::next(_3x, 2)(); // Skip /d
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _5x){
        BOOST_CHECK(request.target() == "/e");
        std::next(_5x, 2)(); // /f is not skipped, path is finished
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/f");
    });

    router.all(".*", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d/e/f/g");
    });

    procs.provide({boost::beast::http::verb::get, "/a/b/c", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d/e", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d/e/f", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b/c/d/e/f/g", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(get_no_2)

BOOST_AUTO_TEST_CASE(get_no_3) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.get("/1/2/3",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/2");
    });

    router.get("/1/2/3/4",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        std::next(_2x)();
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/3");
    });

    router.get("/1/2/3/4/5",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x, 2)(); // Skip /2
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    },[](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/3");
        std::next(_3x)();
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/4");
    });

    router.get("/1/2/3/4/5/6",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        std::next(_2x, 3)(); // Skip /3, /4
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/5");
    });

    router.get("/1/2/3/4/5/6/7",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        std::next(_2x, 2)(); // Skip /3
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _4x){
        BOOST_CHECK(request.target() == "/4");
        std::next(_4x, 2)(); // Skip /5
    }, [](auto /*request*/, auto /*context*/, auto /*_5x*/){
        // Empty
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/6");
    });

    router.get("/1/2/3/4/5/6/7/8",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        std::next(_2x, 6)(); // Skip /3, /4, /5, /6, /7
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_5x*/){
        // Empty
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/8");
    });

    router.get("/1/2/3/4/5/6/7/8/9",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        std::next(_1x, 12)(); // Skip /2, /3, /4, /5, /6, /7, /8
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/9");
    });

    router.all(".*", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "999999999999999");
    });

    procs.provide({boost::beast::http::verb::get, "/1/2/3", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/1/2/3/4", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/1/2/3/4/5", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/1/2/3/4/5/6", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/1/2/3/4/5/6/7", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/1/2/3/4/5/6/7/8", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/1/2/3/4/5/6/7/8/9", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::head, "999999999999999", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(get_no_3)

BOOST_AUTO_TEST_CASE(post_no_1) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.post("/testpath", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/testpath");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/path/to/resource", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/path/to/resource");
        BOOST_CHECK(request.body() == "postdata");
    });

    procs.provide({boost::beast::http::verb::post, "/testpath", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/path/to/resource", 11, "postdata"}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(post_no_1)

BOOST_AUTO_TEST_CASE(post_no_2) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.post("/a/b/c",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_2x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/c");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/a/b/c/d",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x, 2)(); // Skip /b
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/c");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_3x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/d");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/a/b/c/d/e",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_2x, 3)(); // Skip /c, /d
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/e");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/a/b/c/d/e/f",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x, 2)(); // Skip /b
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/c");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_3x, 2)(); // Skip /d
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _5x){
        BOOST_CHECK(request.target() == "/e");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_5x, 2)(); // /f is not skipped, path is finished
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/f");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.all(".*", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d/e/f/g");
        BOOST_CHECK(request.body() == "postdata");
    });

    procs.provide({boost::beast::http::verb::post, "/a/b/c", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d/e", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d/e/f", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/a/b/c/d/e/f/g", 11, "postdata"}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(post_no_2)

BOOST_AUTO_TEST_CASE(post_no_3) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    router.post("/1/2/3",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/1/2/3/4",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_2x)();
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/3");
    });

    router.post("/1/2/3/4/5",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x, 2)(); // Skip /2
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    },[](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/3");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_3x)();
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/4");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/1/2/3/4/5/6",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_2x, 3)(); // Skip /3, /4
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/5");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/1/2/3/4/5/6/7",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_2x, 2)(); // Skip /3
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _4x){
        BOOST_CHECK(request.target() == "/4");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_4x, 2)(); // Skip /5
    }, [](auto /*request*/, auto /*context*/, auto /*_5x*/){
        // Empty
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/6");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.post("/1/2/3/4/5/6/7/8",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_2x, 6)(); // Skip /3, /4, /5, /6, /7
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_5x*/){
        // Empty
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/8");
    });

    router.post("/1/2/3/4/5/6/7/8/9",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "postdata");
        std::next(_1x, 12)(); // Skip /2, /3, /4, /5, /6, /7, /8
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/9");
        BOOST_CHECK(request.body() == "postdata");
    });

    router.all(".*", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "999999999999999");
        BOOST_CHECK(request.body() == "postdata");
    });

    procs.provide({boost::beast::http::verb::post, "/1/2/3", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/1/2/3/4", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/1/2/3/4/5", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/1/2/3/4/5/6", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/1/2/3/4/5/6/7", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/1/2/3/4/5/6/7/8", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::post, "/1/2/3/4/5/6/7/8/9", 11, "postdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::head, "999999999999999", 11, "postdata"}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(post_no_3)

BOOST_AUTO_TEST_CASE(put_no_1) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    std::string shared_resource;

    router.put("/path/to/resource", [&](auto request, auto /*context*/){
        BOOST_CHECK_EQUAL(request.target(), "/path/to/resource");

        shared_resource = request.body();
    });

    procs.provide({boost::beast::http::verb::put, "/path/to/resource", 11, "putdata1"}, test_session::flesh{});
    BOOST_CHECK_EQUAL(shared_resource, "putdata1");
    procs.provide({boost::beast::http::verb::put, "/path/to/resource", 11, "putdata2"}, test_session::flesh{});
    BOOST_CHECK_EQUAL(shared_resource, "putdata2");

} // BOOST_AUTO_TEST_CASE(put_no_1)

BOOST_AUTO_TEST_CASE(put_no_2) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    std::string shared_resource[4];

    router.put("/a/b/c",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "putdata1");
        std::next(_1x)();
    }, [&](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(request.body(), "putdata1");
        shared_resource[0] = request.body();
        std::next(_2x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/c");
        BOOST_CHECK_EQUAL(request.body(), "putdata1");
    });

    router.put("/a/b/c/d",
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "putdata2");
        std::next(_1x, 2)(); // Skip /b
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/c");
        BOOST_CHECK_EQUAL(request.body(), "putdata2");
        std::next(_3x)();
    }, [&](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/d");
        BOOST_CHECK_EQUAL(request.body(), "putdata2");
        shared_resource[1] = request.body();
    });

    router.put("/a/b/c/d/e",
       [](auto request, auto /*context*/, auto _1x) {
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "putdata3");
        std::next(_1x)();
    }, [&](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/b");
        BOOST_CHECK_EQUAL(request.body(), "putdata3");
        shared_resource[2] = request.body();
        std::next(_2x, 3)(); // Skip /c, /d
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/e");
        BOOST_CHECK_EQUAL(request.body(), "putdata3");
    });

    router.put("/a/b/c/d/e/f",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        BOOST_CHECK_EQUAL(request.body(), "putdata4");
        shared_resource[3] = request.body();
        std::next(_1x, 2)(); // Skip /b
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/c");
        BOOST_CHECK_EQUAL(request.body(), "putdata4");
        std::next(_3x, 2)(); // Skip /d
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _5x){
        BOOST_CHECK(request.target() == "/e");
        BOOST_CHECK_EQUAL(request.body(), "putdata4");
        std::next(_5x, 2)(); // /f is not skipped, path is finished
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/f");
        BOOST_CHECK_EQUAL(request.body(), "putdata4");
    });

    router.all(".*", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a/b/c/d/e/f/g");
        BOOST_CHECK_EQUAL(request.body(), "putdata5");
    });

    procs.provide({boost::beast::http::verb::put, "/a/b/c", 11, "putdata1"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d", 11, "putdata2"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d/e", 11, "putdata3"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d/e/f", 11, "putdata4"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/a/b/c/d/e/f/g", 11, "putdata5"}, test_session::flesh{});
    BOOST_CHECK_EQUAL(shared_resource[0], "putdata1");
    BOOST_CHECK_EQUAL(shared_resource[1], "putdata2");
    BOOST_CHECK_EQUAL(shared_resource[2], "putdata3");
    BOOST_CHECK_EQUAL(shared_resource[3], "putdata4");

} // BOOST_AUTO_TEST_CASE(put_no_2)

BOOST_AUTO_TEST_CASE(put_no_3) {

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    std::string shared_resource[7];

    router.put("/1/2/3",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[0] = request.body();
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "putdata");
    });

    router.put("/1/2/3/4",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[1] = request.body();
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "putdata");
        std::next(_2x)();
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/3");
    });

    router.put("/1/2/3/4/5",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[2] = request.body();
        std::next(_1x, 2)(); // Skip /2
    }, [](auto /*request*/, auto /*context*/, auto /*_2x*/){
        // Empty
    },[](auto request, auto /*context*/, auto _3x){
        BOOST_CHECK(request.target() == "/3");
        BOOST_CHECK(request.body() == "putdata");
        std::next(_3x)();
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/4");
        BOOST_CHECK(request.body() == "putdata");
    });

    router.put("/1/2/3/4/5/6",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[3] = request.body();
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "putdata");
        std::next(_2x, 3)(); // Skip /3, /4
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/5");
        BOOST_CHECK(request.body() == "putdata");
    });

    router.put("/1/2/3/4/5/6/7",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[4] = request.body();
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "putdata");
        std::next(_2x, 2)(); // Skip /3
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto request, auto /*context*/, auto _4x){
        BOOST_CHECK(request.target() == "/4");
        BOOST_CHECK(request.body() == "putdata");
        std::next(_4x, 2)(); // Skip /5
    }, [](auto /*request*/, auto /*context*/, auto /*_5x*/){
        // Empty
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/6");
        BOOST_CHECK(request.body() == "putdata");
    });

    router.put("/1/2/3/4/5/6/7/8",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[5] = request.body();
        std::next(_1x)();
    }, [](auto request, auto /*context*/, auto _2x){
        BOOST_CHECK(request.target() == "/2");
        BOOST_CHECK(request.body() == "putdata");
        std::next(_2x, 6)(); // Skip /3, /4, /5, /6, /7
    }, [](auto /*request*/, auto /*context*/, auto /*_3x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_4x*/){
        // Empty
    }, [](auto /*request*/, auto /*context*/, auto /*_5x*/){
        // Empty
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/8");
    });

    router.put("/1/2/3/4/5/6/7/8/9",
       [&](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/1");
        BOOST_CHECK(request.body() == "putdata");
        shared_resource[6] = request.body();
        std::next(_1x, 12)(); // Skip /2, /3, /4, /5, /6, /7, /8
    }, [](auto request, auto /*context*/) {
        BOOST_CHECK(request.target() == "/9");
        BOOST_CHECK(request.body() == "putdata");
    });

    router.all(".*", [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "999999999999999");
        BOOST_CHECK(request.body() == "putdata");
    });

    procs.provide({boost::beast::http::verb::put, "/1/2/3", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/1/2/3/4", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/1/2/3/4/5", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/1/2/3/4/5/6", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/1/2/3/4/5/6/7", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/1/2/3/4/5/6/7/8", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::put, "/1/2/3/4/5/6/7/8/9", 11, "putdata"}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::head, "999999999999999", 11, "putdata"}, test_session::flesh{});
    BOOST_CHECK_EQUAL(shared_resource[0], "putdata");
    BOOST_CHECK_EQUAL(shared_resource[1], "putdata");
    BOOST_CHECK_EQUAL(shared_resource[2], "putdata");
    BOOST_CHECK_EQUAL(shared_resource[3], "putdata");
    BOOST_CHECK_EQUAL(shared_resource[4], "putdata");
    BOOST_CHECK_EQUAL(shared_resource[5], "putdata");
    BOOST_CHECK_EQUAL(shared_resource[6], "putdata");

} // BOOST_AUTO_TEST_CASE(put_no_3)

BOOST_AUTO_TEST_CASE(literals_no_1) {

    using http::literals::operator""_get;

    http::basic_router<test_session> router{regex_flags};

    http::base::request_processor<test_session>
            procs{router.resource_map(), router.method_map(), router.regex_flags()};

    "^/a$"_get.advance(router, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/a");
    });

    "^/a/b$"_get.advance(router,
       [](auto request, auto /*context*/, auto _1x){
        BOOST_CHECK(request.target() == "/a");
        std::next(_1x)();
    }, [](auto request, auto /*context*/){
        BOOST_CHECK(request.target() == "/b");
    });

    procs.provide({boost::beast::http::verb::get, "/a", 11}, test_session::flesh{});
    procs.provide({boost::beast::http::verb::get, "/a/b", 11}, test_session::flesh{});

} // BOOST_AUTO_TEST_CASE(literals_no_1)
