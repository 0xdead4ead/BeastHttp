#define BOOST_TEST_MODULE display_test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <base/display.hxx>

#include <sstream>

using namespace _0xdead4ead;

BOOST_AUTO_TEST_CASE(no_1) {

    {
        std::ostringstream os;
        http::base::display::print(os, 1, 2, 3, 4);
        BOOST_CHECK_EQUAL(os.str(), "1234");
    }

    {
        std::ostringstream os;
        http::base::display::print<' '>(os, 1, 2, 3, 4);
        http::base::display::print(os, ' ');
        http::base::display::print<','>(os, 5, 6, 7, 8);
        BOOST_CHECK_EQUAL(os.str(), "1 2 3 4 5,6,7,8");
    }

    {
        std::ostringstream os;
        http::base::display::print(os, 'a', 'b', 'r', 'a', "cadabra");
        BOOST_CHECK_EQUAL(os.str(), "abracadabra");
    }

    {
        std::ostringstream os;
        http::base::display::print(os, 25, '$');
        http::base::display::print(os, ',', ' ');
        http::base::display::print(os, 50, '$');
        BOOST_CHECK_EQUAL(os.str(), "25$, 50$");
    }

} // BOOST_AUTO_TEST_CASE(no_1)

BOOST_AUTO_TEST_CASE(no_2) {

    {
        std::ostringstream os;
        http::base::display::printline(os, 1, 2, 3, 4);
        BOOST_CHECK_EQUAL(os.str(), "1234\n");
    }

    {
        std::ostringstream os;
        http::base::display::print<' '>(os, 1, 2, 3, 4);
        http::base::display::print(os, ' ');
        http::base::display::printline<','>(os, 5, 6, 7, 8);
        BOOST_CHECK_EQUAL(os.str(), "1 2 3 4 5,6,7,8\n");
    }

    {
        std::ostringstream os;
        http::base::display::printline(os, 'a', 'b', 'r', 'a', "cadabra");
        BOOST_CHECK_EQUAL(os.str(), "abracadabra\n");
    }

    {
        std::ostringstream os;
        http::base::display::print(os, 25, '$');
        http::base::display::print(os, ',', ' ');
        http::base::display::printline(os, 50, '$');
        BOOST_CHECK_EQUAL(os.str(), "25$, 50$\n");
    }

} // BOOST_AUTO_TEST_CASE(no_2)
