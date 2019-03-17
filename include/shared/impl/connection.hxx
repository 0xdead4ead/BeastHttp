#if not defined BEASTHTTP_SHARED_IMPL_CONNECTION_HXX
#define BEASTHTTP_SHARED_IMPL_CONNECTION_HXX

namespace _0xdead4ead {
namespace http {
namespace shared {

template<class Protocol,
         template<typename> class Socket>
connection<Protocol, Socket>::connection(socket_type&& socket)
    : base_connection{socket.get_executor()},
      base_socket{std::move(socket)}
{
}

template<class Protocol,
         template<typename> class Socket>
boost::beast::error_code
connection<Protocol, Socket>::shutdown(shutdown_type type)
{
    return base_socket::shutdown(type);
}

template<class Protocol,
         template<typename> class Socket>
boost::beast::error_code
connection<Protocol, Socket>::close()
{
    return base_socket::close();
}

template<class Protocol,
         template<typename> class Socket>
typename connection<Protocol, Socket>::socket_type&
connection<Protocol, Socket>::stream()
{
    return base_socket::instance_;
}

template<class Protocol,
         template<typename> class Socket>
typename connection<Protocol, Socket>::socket_type
connection<Protocol, Socket>::release_socket()
{
    return base_socket::release();
}

} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_SHARED_IMPL_CONNECTION_HXX
