#if not defined BEASTHTTP_COMMON_IMPL_CONNECTION_HXX
#define BEASTHTTP_COMMON_IMPL_CONNECTION_HXX

namespace _0xdead4ead {
namespace http {
namespace common {

template<class Socket,
         class CompletionExecutor>
connection<Socket, CompletionExecutor>::connection(
        socket_type&& socket,
        const CompletionExecutor& completion_executor)
    : base_connection{completion_executor},
      socket_{std::move(socket)}
{
}

template<class Socket,
         class CompletionExecutor>
boost::beast::error_code
connection<Socket, CompletionExecutor>::shutdown(shutdown_type type)
{
    auto ec = boost::system::error_code{};
    socket_.shutdown(type, ec);

    return ec;
}

template<class Socket,
         class CompletionExecutor>
boost::beast::error_code
connection<Socket, CompletionExecutor>::close()
{
    auto ec = boost::system::error_code{};
    socket_.close(ec);

    return ec;
}

template<class Socket,
         class CompletionExecutor>
typename connection<Socket, CompletionExecutor>::socket_type&
connection<Socket, CompletionExecutor>::asio_socket()
{
    return socket_;
}

template<class Socket,
         class CompletionExecutor>
typename connection<Socket, CompletionExecutor>::socket_type&
connection<Socket, CompletionExecutor>::stream()
{
    return socket_;
}

} // namespace common
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_COMMON_IMPL_CONNECTION_HXX
