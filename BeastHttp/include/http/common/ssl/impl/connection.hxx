#if not defined BEASTHTTP_COMMON_SSL_IMPL_CONNECTION_HXX
#define BEASTHTTP_COMMON_SSL_IMPL_CONNECTION_HXX

namespace _0xdead4ead {
namespace http {
namespace common {
namespace ssl {

template<class Socket,
         class CompletionExecutor>
connection<Socket, CompletionExecutor>::connection(
        socket_type&& socket, boost::asio::ssl::context& ctx,
        const CompletionExecutor& completion_executor)
    : base_connection{completion_executor},
      stream_{std::move(socket), ctx}
{
}

template<class Socket,
         class CompletionExecutor>
template <class F, class B>
void
connection<Socket, CompletionExecutor>::async_handshake(
        const B& buf, F&& f)
{
    stream_.async_handshake(
                boost::asio::ssl::stream_base::server, buf.data(),
                boost::asio::bind_executor(base_connection::completion_executor_,
                                           std::forward<F>(f)));
}

template<class Socket,
         class CompletionExecutor>
template<class F>
void
connection<Socket, CompletionExecutor>::async_shutdown(F&& f)
{
    stream_.async_shutdown(
                boost::asio::bind_executor(base_connection::completion_executor_,
                                           std::forward<F>(f)));
}

template<class Socket,
         class CompletionExecutor>
boost::beast::error_code
connection<Socket, CompletionExecutor>::force_shutdown(shutdown_type type)
{
    auto ec = boost::system::error_code{};
    stream_.next_layer().shutdown(type, ec);

    return ec;
}

template<class Socket,
         class CompletionExecutor>
boost::beast::error_code
connection<Socket, CompletionExecutor>::force_close()
{
    auto ec = boost::system::error_code{};
    stream_.next_layer().close(ec);

    return ec;
}

template<class Socket,
         class CompletionExecutor>
typename connection<Socket, CompletionExecutor>::ssl_stream_type&
connection<Socket, CompletionExecutor>::beast_ssl_stream()
{
    return stream_;
}

template<class Socket,
         class CompletionExecutor>
typename connection<Socket, CompletionExecutor>::socket_type&
connection<Socket, CompletionExecutor>::asio_socket()
{
    return stream_.next_layer();
}

template<class Socket,
         class CompletionExecutor>
typename connection<Socket, CompletionExecutor>::ssl_stream_type&
connection<Socket, CompletionExecutor>::stream()
{
    return stream_;
}

} // namespace ssl
} // namespace common
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_COMMON_SSL_IMPL_CONNECTION_HXX
