#if not defined BEASTHTTP_OUT_HXX
#define BEASTHTTP_OUT_HXX

#include <http/base/display.hxx>
#include <http/base/version.hxx>

#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/version.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace _0xdead4ead {
namespace http {
namespace out {

constexpr char whitespace = ' ';

namespace asio = boost::asio;

namespace prefix {
namespace version {

template<class Stream, class Destination, class... Args>
auto push(Destination& destination, Args const&... args)
-> decltype (Stream(std::addressof(std::declval<asio::streambuf&>())),
             asio::write(destination, std::declval<asio::streambuf&>()))
{
    asio::streambuf buffer{};
    Stream stream{std::addressof(buffer)};
    base::display::print(stream, "BeastHttp/", BEASTHTTP_VERSION_VALUE, whitespace,
                         '[', BOOST_BEAST_VERSION_STRING, ']', whitespace);
    base::display::print<whitespace>(stream, args...);
    return asio::write(destination, buffer);
}

template<class Stream, class Destination, class... Args>
auto pushn(Destination& destination, Args const&... args)
-> decltype (Stream(std::addressof(std::declval<asio::streambuf&>())),
             asio::write(destination, std::declval<asio::streambuf&>()))
{
    asio::streambuf buffer{};
    Stream stream{std::addressof(buffer)};
    base::display::print(stream, "BeastHttp/", BEASTHTTP_VERSION_VALUE, whitespace,
                         '[', BOOST_BEAST_VERSION_STRING, ']', whitespace);
    base::display::printline<whitespace>(stream, args...);
    return asio::write(destination, buffer);
}

namespace time {

template<class Stream, class Destination, class... Args>
auto push(Destination& destination, Args const&... args)
-> decltype (Stream(std::addressof(std::declval<asio::streambuf&>())),
             asio::write(destination, std::declval<asio::streambuf&>()))
{
    asio::streambuf buffer{};
    Stream stream{std::addressof(buffer)};
    base::display::print(stream, "BeastHttp/", BEASTHTTP_VERSION_VALUE, whitespace,
                         '[', BOOST_BEAST_VERSION_STRING, ']', whitespace,
                         '(', boost::posix_time::second_clock::local_time(), ')', whitespace);
    base::display::print<whitespace>(stream, args...);
    return asio::write(destination, buffer);
}

template<class Stream, class Destination, class... Args>
auto pushn(Destination& destination, Args const&... args)
-> decltype (Stream(std::addressof(std::declval<asio::streambuf&>())),
             asio::write(destination, std::declval<asio::streambuf&>()))
{
    asio::streambuf buffer{};
    Stream stream{std::addressof(buffer)};
    base::display::print(stream, "BeastHttp/", BEASTHTTP_VERSION_VALUE, whitespace,
                         '[', BOOST_BEAST_VERSION_STRING, ']', whitespace,
                         '(', boost::posix_time::second_clock::local_time(), ')', whitespace);
    base::display::printline<whitespace>(stream, args...);
    return asio::write(destination, buffer);
}

} // namespace time
} // namespace version
} // namespace prefix

template<class Stream, class Destination, class... Args>
auto push(Destination& destination, Args const&... args)
-> decltype (Stream(std::addressof(std::declval<asio::streambuf&>())),
             asio::write(destination, std::declval<asio::streambuf&>()))
{
    asio::streambuf buffer{};
    Stream stream{std::addressof(buffer)};
    base::display::print<whitespace>(stream, args...);
    return asio::write(destination, buffer);
}

template<class Stream, class Destination, class... Args>
auto pushn(Destination& destination, Args const&... args)
-> decltype (Stream(std::addressof(std::declval<asio::streambuf&>())),
             asio::write(destination, std::declval<asio::streambuf&>()))
{
    asio::streambuf buffer{};
    Stream stream{std::addressof(buffer)};
    base::display::printline<whitespace>(stream, args...);
    return asio::write(destination, buffer);
}

} // namespace out
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_OUT_HXX
