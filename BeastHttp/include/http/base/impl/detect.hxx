#if not defined BEASTHTTP_BASE_IMPL_DETECT_HXX
#define BEASTHTTP_BASE_IMPL_DETECT_HXX

#include <boost/beast/core/detect_ssl.hpp>

#include <boost/asio/bind_executor.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class CompletionExecutor>
detect<CompletionExecutor>::detect(CompletionExecutor const& completion_executor)
    : completion_executor_{completion_executor}
{
}

template<class CompletionExecutor>
template<class S, class B, class F>
void
detect<CompletionExecutor>::async(S& s, B& buf, F&& f)
{
    boost::beast::async_detect_ssl(s, buf,
                     boost::asio::bind_executor(
                         completion_executor_, std::forward<F>(f)));
}

template<class CompletionExecutor>
template<class S, class B>
boost::beast::error_code
detect<CompletionExecutor>::sync(S& s, B& buf, boost::tribool& res)
{
    auto ec = boost::beast::error_code{};

    res = boost::beast::detect_ssl(s, buf, ec);

    return ec;
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_DETECT_HXX
