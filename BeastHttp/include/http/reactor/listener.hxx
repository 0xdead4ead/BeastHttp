#if not defined BEASTHTTP_REACTOR_LISTENER_HXX
#define BEASTHTTP_REACTOR_LISTENER_HXX

#include <http/base/traits.hxx>
#include <http/base/strand_stream.hxx>

#include <boost/asio/io_context.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/utility/string_view.hpp>

#define BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES \
    OnAccept, OnError, Protocol, Acceptor, Socket, Endpoint

namespace _0xdead4ead {
namespace http {
namespace reactor {

template<template<typename> class OnAccept = std::function,
         template<typename> class OnError = std::function,
         class Protocol = boost::asio::ip::tcp,
         class Acceptor = boost::asio::basic_socket_acceptor<Protocol>,
         class Socket = boost::asio::basic_stream_socket<Protocol>,
         template<typename> class Endpoint = boost::asio::ip::basic_endpoint>
class listener : private base::strand_stream,
        public std::enable_shared_from_this<listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>>
        , boost::asio::coroutine
{
public:

    using self_type = listener;

    using protocol_type = Protocol;

    using acceptor_type = Acceptor;

    using socket_type = Socket;

    using asio_executor_type = typename socket_type::executor_type;

    using endpoint_type = Endpoint<protocol_type>;

    using on_accept_type = OnAccept<void (socket_type)>;

    using on_error_type = OnError<void (boost::system::error_code, boost::string_view)>;

    static_assert (base::traits::TryInvoke<on_accept_type, void(socket_type)>::value,
                   "Invalid OnAccept handler type!");

    static_assert (base::traits::TryInvoke<on_error_type, void(boost::system::error_code, boost::string_view)>::value,
                   "Invalid OnError handler type!");

    template<class... _OnAction>
    static auto
    launch(boost::asio::io_context& ctx, const endpoint_type& endpoint,
           _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)));

    template<class... _OnAction>
    static auto
    launch(boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx,
           const endpoint_type& endpoint, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<boost::asio::io_context&>(),
                           std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)));

    template<class Deleter, class Allocator, class... _OnAction>
    static auto
    launch(boost::asio::io_context& ctx, const Deleter& d, const Allocator& alloc,
           const endpoint_type& endpoint, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)));

    template<class Deleter, class Allocator, class... _OnAction>
    static auto
    launch(boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx,
           const Deleter& d, const Allocator& alloc, const endpoint_type& endpoint,
           _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<boost::asio::io_context&>(),
                           std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)));

    template<class _OnAccept>
    explicit
    listener(boost::asio::io_context& ctx, _OnAccept&& on_accept,
             typename std::enable_if<base::traits::TryInvoke<
             _OnAccept, void(socket_type)>::value, int>::type = 0);

    template<class _OnAccept, class _OnError>
    explicit
    listener(boost::asio::io_context& ctx, _OnAccept&& on_accept, _OnError&& on_error,
             typename std::enable_if<base::traits::TryInvoke<_OnAccept, void(socket_type)>::value and
             base::traits::TryInvoke<_OnError, void(
                 boost::system::error_code, boost::string_view)>::value, int>::type = 0);

    template<class _OnAccept>
    explicit
    listener(boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx,
             _OnAccept&& on_accept, typename std::enable_if<base::traits::TryInvoke<
             _OnAccept, void(socket_type)>::value, int>::type = 0);

    template<class _OnAccept, class _OnError>
    explicit
    listener(boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx,
             _OnAccept&& on_accept, _OnError&& on_error, typename std::enable_if<base::traits::TryInvoke<
             _OnAccept, void(socket_type)>::value and base::traits::TryInvoke<_OnError, void(
                 boost::system::error_code, boost::string_view)>::value, int>::type = 0);

private:

    void
    on_spawn_connect(boost::system::error_code ec, socket_type& socket);

    void
    on_accept(boost::system::error_code ec, socket_type socket);

    void
    loop(const endpoint_type& endpoint);

    void
    do_accept();

    acceptor_type acceptor_;
    boost::asio::io_context& socket_ctx_;

    endpoint_type endpoint_;
    on_accept_type on_accept_;
    on_error_type on_error_;

}; // class listener

namespace _default {

using listener_type = listener<>;

} // namespace _default
} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#include <http/reactor/impl/listener.hxx>

#endif // not defined BEASTHTTP_REACTOR_LISTENER_HXX
