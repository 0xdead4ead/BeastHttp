#if not defined BEASTHTTP_COMMON_CONNECTION_HXX
#define BEASTHTTP_COMMON_CONNECTION_HXX

#include <http/base/connection.hxx>

#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>
#include <boost/noncopyable.hpp>

namespace _0xdead4ead {
namespace http {
namespace common {

template<class Socket,
         class CompletionExecutor>
class connection :
        public base::connection<connection<Socket, CompletionExecutor>,
        CompletionExecutor>, boost::noncopyable
{
    using self_type = connection;

    using base_connection = base::connection<self_type, CompletionExecutor>;

public:

    using socket_type = Socket;

    using shutdown_type = typename socket_type::shutdown_type;

    explicit
    connection(socket_type&&, const CompletionExecutor&);

    boost::beast::error_code
    shutdown(shutdown_type);

    boost::beast::error_code
    close();

    socket_type&
    asio_socket();

    socket_type&
    stream();

    socket_type socket_;

}; // class connection

} // namespace common
} // namespace http
} // namespace _0xdead4ead

#include <http/common/impl/connection.hxx>

#endif // not defined BEASTHTTP_COMMON_CONNECTION_HXX
