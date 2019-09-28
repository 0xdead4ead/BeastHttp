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
template <class Function, class Serializer>
void
connection<Derived, CompletionExecutor>::async_write(Serializer& serializer, Function&& function)
{
    boost::beast::http::async_write(derived().stream(), serializer,
                               boost::asio::bind_executor(
                                       completion_executor_, std::forward<Function>(function)));
}

template<class Derived, class CompletionExecutor>
template <class Function, class Buffer, class Parser>
void
connection<Derived, CompletionExecutor>::async_read(Buffer& buffer, Parser& parser, Function&& function)
{
    boost::beast::http::async_read(derived().stream(), buffer, parser,
                               boost::asio::bind_executor(
                                       completion_executor_, std::forward<Function>(function)));
}

template<class Derived, class CompletionExecutor>
template<class Serializer>
boost::beast::error_code
connection<Derived, CompletionExecutor>::write(Serializer& serializer)
{
    auto ec = boost::beast::error_code{};

    boost::beast::http::write(derived().stream(), serializer, ec);

    return ec;
}

template<class Derived, class CompletionExecutor>
template<class Parser, class Buffer>
boost::beast::error_code
connection<Derived, CompletionExecutor>::read(Buffer& buffer, Parser& parser)
{
    auto ec = boost::beast::error_code{};

    boost::beast::http::read(derived().stream(), buffer, parser, ec);

    return ec;
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_CONNECTION_HXX
