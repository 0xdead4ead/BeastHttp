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

    template <class Function, class Serializer>
    void
    async_write(Serializer&, Function&&);

    template <class Function, class Buffer, class Parser>
    void
    async_read(Buffer&, Parser&, Function&&);

    template<class Serializer>
    boost::beast::error_code
    write(Serializer&);

    template<class Parser, class Buffer>
    boost::beast::error_code
    read(Buffer&, Parser&);

}; // connection class

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <http/base/impl/connection.hxx>

#endif // not defined BEASTHTTP_BASE_CONNECTION_HXX
