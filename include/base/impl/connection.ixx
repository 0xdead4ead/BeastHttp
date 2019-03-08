#if not defined BEASTHTTP_BASE_CONNECTION_IXX
#define BEASTHTTP_BASE_CONNECTION_IXX

#include <boost/asio/bind_executor.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Derived>
connection<Derived>::connection(io_context::executor_type executor)
    : strand_{executor}
{
}

template<class Derived>
template <class F, class R>
void
connection<Derived>::async_write(/*const*/ R& msg, F&& f)
{
    boost::beast::http::async_write(derived().stream(), msg,
                               boost::asio::bind_executor(
                                       strand_, std::forward<F>(f)));
}

template<class Derived>
template <class F, class B, class R>
void
connection<Derived>::async_read(B& buf, R& msg, F&& f)
{
    boost::beast::http::async_read(derived().stream(), buf, msg,
                               boost::asio::bind_executor(
                                       strand_, std::forward<F>(f)));
}

template<class Derived>
template<class R>
boost::beast::error_code
connection<Derived>::write(/*const*/ R& msg)
{
    auto ec = boost::beast::error_code{};

    boost::beast::http::write(derived().stream(), msg, ec);

    return ec;
}

template<class Derived>
template<class R, class B>
boost::beast::error_code
connection<Derived>::read(B& buf, R& msg)
{
    auto ec = boost::beast::error_code{};

    boost::beast::http::read(derived().stream(), buf, msg, ec);

    return ec;
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_CONNECTION_IXX
