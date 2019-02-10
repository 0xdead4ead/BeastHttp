#if not defined BEASTHTTP_REACTOR_CONNECTION_HXX
#define BEASTHTTP_REACTOR_CONNECTION_HXX

#include "base/connection.hxx"
#include "base/socket.hxx"

#define BEASTHTTP_DECLARE_FRIEND_SESSION_CLASS \
    template<class, class, class, template<typename> class, class, \
             template<typename, typename...> class, template<typename> class, \
             template<typename, typename...> class, template<typename, typename, typename...> class, \
             template<typename, typename, typename...> class, template<typename> class, template<typename> class> \
    friend class session;

namespace http {
namespace reactor {

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

    BEASTHTTP_DECLARE_FRIEND_SESSION_CLASS

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

private:

    explicit
    connection(socket_type&& socket)
        : base_connection{socket.get_executor()},
          base_socket{std::move(socket)}
    {}

    ~connection() override
    {
        close();
    }

}; // class connection

} // namespace reactor
} // namespace http

#endif // not defined BEASTHTTP_REACTOR_CONNECTION_HXX
