#if not defined BEASTHTTP_SHARED_SSL_CONNECTION_HXX
#define BEASTHTTP_SHARED_SSL_CONNECTION_HXX

#include <base/connection.hxx>
#include <base/socket.hxx>

#include <boost/asio/ssl/stream.hpp>

namespace _0xdead4ead {
namespace http {
namespace shared {
namespace ssl {

/**
  @brief The ssl connection class
*/
template<class Protocol,
         template<typename> class Socket>
class connection : private base::socket<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES>,
        public base::connection<connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES>>
{
    using self_type = connection;

    using base_connection = base::connection<self_type>;

    using base_socket = base::socket<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES>;

public:

    using socket_type = typename base_socket::Sock;

    using ssl_stream_type = boost::asio::ssl::stream<socket_type&>;

    using shutdown_type = typename socket_type::shutdown_type;

    explicit
    connection(socket_type&&, boost::asio::ssl::context&);

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
} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#include <shared/ssl/impl/connection.hxx>

#endif // not defined BEASTHTTP_SHARED_SSL_CONNECTION_HXX
