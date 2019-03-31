#if not defined BEASTHTTP_REACTOR_SSL_IMPL_CONNECTION_HXX
#define BEASTHTTP_REACTOR_SSL_IMPL_CONNECTION_HXX

namespace _0xdead4ead {
namespace http {
namespace shared {
namespace ssl {

template<class Protocol,
         template<typename> class Socket>
connection<Protocol, Socket>::connection(socket_type&& socket,
                                         boost::asio::ssl::context& ctx)
    : base_socket{std::move(socket)},
      base_connection{base_socket::instance_.get_executor()},
      stream_{base_socket::instance_, ctx}
{
}

template<class Protocol,
         template<typename> class Socket>
template <class F, class B>
void
connection<Protocol, Socket>::async_handshake(const B& buf, F&& f)
{
    stream_.async_handshake(
                boost::asio::ssl::stream_base::server, buf.data(),
                boost::asio::bind_executor(base_connection::strand_,
                                           std::forward<F>(f)));
}

template<class Protocol,
         template<typename> class Socket>
template<class F>
void
connection<Protocol, Socket>::async_shutdown(F&& f)
{
    stream_.async_shutdown(
                boost::asio::bind_executor(base_connection::strand_,
                                           std::forward<F>(f)));
}

template<class Protocol,
         template<typename> class Socket>
boost::beast::error_code
connection<Protocol, Socket>::force_shutdown(shutdown_type type)
{
    return base_socket::shutdown(type);
}

template<class Protocol,
         template<typename> class Socket>
boost::beast::error_code
connection<Protocol, Socket>::force_close()
{
    return base_socket::close();
}

template<class Protocol,
         template<typename> class Socket>
typename connection<Protocol, Socket>::ssl_stream_type&
connection<Protocol, Socket>::stream()
{
    return stream_;
}

template<class Protocol,
         template<typename> class Socket>
typename connection<Protocol, Socket>::socket_type&
connection<Protocol, Socket>::socket()
{
    return base_socket::instance_;
}

} // namespace ssl
} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_SSL_IMPL_CONNECTION_HXX
