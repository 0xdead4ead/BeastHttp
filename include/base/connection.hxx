#if not defined BEASTHTTP_BASE_CONNECTION_HXX
#define BEASTHTTP_BASE_CONNECTION_HXX

#include <boost/beast/core/error.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Derived, class CompletionExecutor>
class connection
{
    using self_type = connection;

    Derived&
    derived()
    {
        return static_cast<Derived&>(*this);
    }

protected:

    CompletionExecutor const& completion_executor_;

    explicit
    connection(CompletionExecutor const&);

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
