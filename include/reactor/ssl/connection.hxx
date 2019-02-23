#if not defined BEASTHTTP_REACTOR_SSL_CONNECTION_HXX
#define BEASTHTTP_REACTOR_SSL_CONNECTION_HXX

#include "base/connection.hxx"
#include "base/socket.hxx"

#include <boost/asio/ssl/stream.hpp>

#define BEASTHTTP_DECLARE_FRIEND_SSL_SESSION_CLASS \
    template<class, class, class, template<typename> class, class, \
             template<typename, typename...> class, template<typename> class, \
             template<typename, typename...> class, template<typename, typename, typename...> class, \
             template<typename, typename, typename...> class, template<typename> class, template<typename> class, template<typename> class> \
    friend class session;

namespace http {
namespace reactor {
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

    using socket_type = typename base_socket::Sock;

    using ssl_stream_type = boost::asio::ssl::stream<socket_type&>;

    using shutdown_type = typename socket_type::shutdown_type;

public:

    BEASTHTTP_DECLARE_FRIEND_SSL_SESSION_CLASS

    template <class F, class B>
    void
    async_handshake(const B& buf, F&& f)
    {
        stream_.async_handshake(
                    boost::asio::ssl::stream_base::server, buf.data(),
                    boost::asio::bind_executor(base_connection::strand_, std::forward<F>(f)));
    }

    template<class F>
    void
    async_shutdown(F&& f)
    {
        stream_.async_shutdown(
                    boost::asio::bind_executor(base_connection::strand_, std::forward<F>(f)));
    }

    boost::beast::error_code
    force_shutdown(shutdown_type type)
    {
        return base_socket::shutdown(type);
    }

    boost::beast::error_code
    force_close()
    {
        return base_socket::close();
    }

    ssl_stream_type&
    stream()
    {
        return stream_;
    }

    socket_type
    release_socket()
    {
        return base_socket::release_socket();
    }

private:

    explicit
    connection(socket_type&& socket, boost::asio::ssl::context& ctx)
        : base_socket{std::move(socket)},
          base_connection{base_socket::instance_.get_executor()},
          stream_{base_socket::instance_, ctx}
    {}

    ~connection() override
    {}

    ssl_stream_type stream_;

}; // class connection

} // namespace ssl
} // namespace reactor
} // namespace http

#endif // not defined BEASTHTTP_REACTOR_SSL_CONNECTION_HXX
