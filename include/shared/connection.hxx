#if not defined BEASTHTTP_SHARED_CONNECTION_HXX
#define BEASTHTTP_SHARED_CONNECTION_HXX

#include <base/connection.hxx>
#include <base/socket.hxx>

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
    connection(socket_type&&);

    connection(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    boost::beast::error_code
    shutdown(shutdown_type);

    boost::beast::error_code
    close();

    socket_type&
    stream();

}; // class connection

} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#include <shared/impl/connection.hxx>

#endif // not defined BEASTHTTP_SHARED_CONNECTION_HXX
