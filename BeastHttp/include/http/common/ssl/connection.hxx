#if not defined BEASTHTTP_COMMON_SSL_CONNECTION_HXX
#define BEASTHTTP_COMMON_SSL_CONNECTION_HXX

#include <http/base/beast/ssl_stream.hpp>
#include <http/base/connection.hxx>

#include <boost/noncopyable.hpp>

namespace _0xdead4ead {
namespace http {
namespace common {
namespace ssl {

template<class Socket,
         class CompletionExecutor>
class connection :
        public base::connection<connection<Socket, CompletionExecutor>,
        CompletionExecutor>, boost::noncopyable
{
    using self_type = connection;

    using base_connection = base::connection<self_type, CompletionExecutor>;

public:

    using socket_type = typename std::remove_reference<Socket>::type;

    using ssl_stream_type = boost::beast::ssl_stream<socket_type>;

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
    beast_ssl_stream();

    socket_type&
    asio_socket();

    ssl_stream_type&
    stream();

private:

    ssl_stream_type stream_;

}; // class connection

} // namespace ssl
} // namespace common
} // namespace http
} // namespace _0xdead4ead

#include <http/common/ssl/impl/connection.hxx>

#endif // not defined BEASTHTTP_COMMON_SSL_CONNECTION_HXX
