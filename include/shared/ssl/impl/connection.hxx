#if not defined BEASTHTTP_REACTOR_SSL_IMPL_CONNECTION_HXX
#define BEASTHTTP_REACTOR_SSL_IMPL_CONNECTION_HXX

namespace _0xdead4ead {
namespace http {
namespace shared {
namespace ssl {

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::connection(
        socket_type&& socket, boost::asio::ssl::context& ctx,
        const CompletionExecutor& completion_executor)
    : base_connection{completion_executor},
      base_socket{std::move(socket)},
      stream_{base_socket::instance_, ctx}
{
}

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
template <class F, class B>
void
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::async_handshake(
        const B& buf, F&& f)
{
    stream_.async_handshake(
                boost::asio::ssl::stream_base::server, buf.data(),
                boost::asio::bind_executor(base_connection::completion_executor_,
                                           std::forward<F>(f)));
}

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
template<class F>
void
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::async_shutdown(F&& f)
{
    stream_.async_shutdown(
                boost::asio::bind_executor(base_connection::completion_executor_,
                                           std::forward<F>(f)));
}

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
boost::beast::error_code
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::force_shutdown(shutdown_type type)
{
    return base_socket::shutdown(type);
}

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
boost::beast::error_code
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::force_close()
{
    return base_socket::close();
}

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
typename connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::ssl_stream_type&
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::stream()
{
    return stream_;
}

template<class Protocol,
         template<typename> class Socket,
         class CompletionExecutor>
typename connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::socket_type&
connection<BEASTHTTP_SOCKET_TMPL_ATTRIBUTES, CompletionExecutor>::socket()
{
    return base_socket::instance_;
}

} // namespace ssl
} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_SSL_IMPL_CONNECTION_HXX
