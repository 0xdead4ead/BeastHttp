#if not defined BEASTHTTP_COMMON_SSL_CONNECTION_HXX
#define BEASTHTTP_COMMON_SSL_CONNECTION_HXX

#include <http/base/beast/ssl_stream.hpp>
#include <http/base/connection.hxx>
#include <http/base/socket.hxx>

namespace _0xdead4ead {
namespace http {
namespace common {
namespace ssl {

template<class Socket,
         class CompletionExecutor>
class connection : private base::socket<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES>,
        public base::connection<connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>,
        CompletionExecutor>
{
    using self_type = connection;

    using base_connection = base::connection<self_type, CompletionExecutor>;

    using base_socket = base::socket<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES>;

public:

    using socket_type = typename base_socket::socket_type;

    using ssl_stream_type = boost::beast::ssl_stream<socket_type&>;

    using shutdown_type = typename socket_type::shutdown_type;

    explicit
    connection(socket_type&&, boost::asio::ssl::context&,
               const CompletionExecutor&);

    template <class F, class B>
    void
    async_handshake(const B&, F&&);

    template<class F>
    void
    async_shutdown(F&&);

    boost::beast::error_code
    force_shutdown(shutdown_type);

    boost::beast::error_code
    force_close();

    ssl_stream_type&
    stream();

    socket_type&
    socket();

private:

    ssl_stream_type stream_;

}; // class connection

} // namespace ssl
} // namespace common
} // namespace http
} // namespace _0xdead4ead

#include <http/common/ssl/impl/connection.hxx>

#endif // not defined BEASTHTTP_COMMON_SSL_CONNECTION_HXX
