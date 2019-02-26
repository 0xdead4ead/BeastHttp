#if not defined BEASTHTTP_REACTOR_CONNECTION_HXX
#define BEASTHTTP_REACTOR_CONNECTION_HXX

#include "base/connection.hxx"
#include "base/socket.hxx"

namespace _0xdead4ead {
namespace http {
namespace shared {

/**
  @brief The plain connection class
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

    using shutdown_type = typename socket_type::shutdown_type;

    explicit
    connection(socket_type&& socket)
        : base_connection{socket.get_executor()},
          base_socket{std::move(socket)}
    {}

    connection(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    boost::beast::error_code
    shutdown(shutdown_type type)
    {
        return base_socket::shutdown(type);
    }

    boost::beast::error_code
    close()
    {
        return base_socket::close();
    }

    socket_type&
    stream()
    {
        return base_socket::instance_;
    }

    socket_type
    release_socket()
    {
        return base_socket::release();
    }

}; // class connection

} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_CONNECTION_HXX
