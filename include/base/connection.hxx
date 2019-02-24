#if not defined BEASTHTTP_BASE_CONNECTION_HXX
#define BEASTHTTP_BASE_CONNECTION_HXX

#include "traits.hxx"

#include <boost/system/error_code.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

/**
  @brief The base connection class
*/
template<class Derived>
class connection
{
    using self_type = connection;

    Derived&
    derived()
    {
        return static_cast<Derived&>(*this);
    }

protected:

    using io_context = boost::asio::io_context;

    using strand_type = boost::asio::strand<io_context::executor_type>;

    strand_type strand_;

    explicit
    connection(io_context::executor_type executor)
        : strand_{executor}
    {}

    virtual ~connection() = default; // TODO: extra member

public:

    template <class F, class R>
    void
    async_write(/*const*/ R& msg, F&& f)
    {
        boost::beast::http::async_write(derived().stream(), msg,
                                   boost::asio::bind_executor(
                                           strand_, std::forward<F>(f)));
    }

    template <class F, class B, class R>
    void
    async_read(B& buf, R& msg, F&& f)
    {
        boost::beast::http::async_read(derived().stream(), buf, msg,
                                   boost::asio::bind_executor(
                                           strand_, std::forward<F>(f)));
    }

    template<class R>
    boost::beast::error_code
    write(/*const*/ R& msg)
    {
        auto ec = boost::beast::error_code{};

        boost::beast::http::write(derived().stream(), msg, ec);

        return ec;
    }

    template<class R, class B>
    boost::beast::error_code
    read(B& buf, R& msg)
    {
        auto ec = boost::beast::error_code{};

        boost::beast::http::read(derived().stream(), buf, msg, ec);

        return ec;
    }

}; // connection class

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_CONNECTION_HXX
