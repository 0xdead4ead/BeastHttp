#if not defined BEASTHTTP_BASE_CONNECTION_HXX
#define BEASTHTTP_BASE_CONNECTION_HXX

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/error.hpp>

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
    connection(io_context::executor_type);

public:

    template <class F, class R>
    void
    async_write(/*const*/ R&, F&&);

    template <class F, class B, class R>
    void
    async_read(B&, R&, F&&);

    template<class R>
    boost::beast::error_code
    write(/*const*/ R&);

    template<class R, class B>
    boost::beast::error_code
    read(B&, R&);

}; // connection class

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/connection.hxx>

#endif // not defined BEASTHTTP_BASE_CONNECTION_HXX
