#if not defined BEASTHTTP_BASE_IMPL_CONNECTION_HXX
#define BEASTHTTP_BASE_IMPL_CONNECTION_HXX

#include <boost/asio/bind_executor.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Derived, class CompletionExecutor>
connection<Derived, CompletionExecutor>::connection(CompletionExecutor const& completion_executor)
    : completion_executor_{completion_executor}
{
}

template<class Derived, class CompletionExecutor>
template <class F, class R>
void
connection<Derived, CompletionExecutor>::async_write(/*const*/ R& msg, F&& f)
{
    boost::beast::http::async_write(derived().stream(), msg,
                               boost::asio::bind_executor(
                                       completion_executor_, std::forward<F>(f)));
}

template<class Derived, class CompletionExecutor>
template <class F, class B, class R>
void
connection<Derived, CompletionExecutor>::async_read(B& buf, R& msg, F&& f)
{
    boost::beast::http::async_read(derived().stream(), buf, msg,
                               boost::asio::bind_executor(
                                       completion_executor_, std::forward<F>(f)));
}

template<class Derived, class CompletionExecutor>
template<class R>
boost::beast::error_code
connection<Derived, CompletionExecutor>::write(/*const*/ R& msg)
{
    auto ec = boost::beast::error_code{};

    boost::beast::http::write(derived().stream(), msg, ec);

    return ec;
}

template<class Derived, class CompletionExecutor>
template<class R, class B>
boost::beast::error_code
connection<Derived, CompletionExecutor>::read(B& buf, R& msg)
{
    auto ec = boost::beast::error_code{};

    boost::beast::http::read(derived().stream(), buf, msg, ec);

    return ec;
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_CONNECTION_HXX
