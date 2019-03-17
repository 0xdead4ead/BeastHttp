#if not defined BEASTHTTP_REACTOR_LISTENER_HXX
#define BEASTHTTP_REACTOR_LISTENER_HXX

#include <base/traits.hxx>

#include <boost/asio/io_context.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/coroutine.hpp>

#include <boost/asio/ip/tcp.hpp>

#define BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES \
    OnAccept, OnError, Protocol, Acceptor, Socket, Endpoint

namespace _0xdead4ead {
namespace http {
namespace reactor {

template<template<typename> class OnAccept = std::function,
         template<typename> class OnError = std::function,
         class Protocol = boost::asio::ip::tcp,
         template<typename> class Acceptor = boost::asio::basic_socket_acceptor,
         template<typename> class Socket = boost::asio::basic_stream_socket,
         template<typename> class Endpoint = boost::asio::ip::basic_endpoint>
class listener : public std::enable_shared_from_this<listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>>
        , boost::asio::coroutine
{
    using self_type = listener;

    using io_context = boost::asio::io_context;

public:

    using protocol_type = Protocol;

    using acceptor_type = Acceptor<protocol_type>;

    using socket_type = Socket<protocol_type>;

    using endpoint_type = Endpoint<protocol_type>;

    using on_accept_type = OnAccept<void (socket_type)>;

    using on_error_type = OnError<void (boost::system::error_code, const char*)>;

    static_assert (std::is_base_of<boost::asio::socket_base, socket_type>::value,
                   "Socket type is not supported!");

    static_assert (std::is_base_of<boost::asio::socket_base, acceptor_type>::value,
                   "Acceptor type is not supported!");

    static_assert (base::traits::TryBind<acceptor_type, void(endpoint_type, boost::system::error_code&)>::value,
                   "Invalid Endpoint type");

    static_assert (base::traits::TryInvoke<on_accept_type, void(socket_type)>::value,
                   "Invalid OnAccept handler type!");

    static_assert (base::traits::TryInvoke<on_error_type, void(boost::system::error_code, const char*)>::value,
                   "Invalid OnError handler type!");

    listener(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    template<class... _OnAction>
    static auto
    loop(io_context&, endpoint_type const&, _OnAction&&...) -> decltype (
            self_type{std::declval<io_context&>(), std::declval<_OnAction>()...},
            std::declval<self_type&>());

    boost::system::error_code
    close();

    endpoint_type const&
    endpoint() const;

    socket_type
    socket();

    template<class _OnAccept>
    explicit
    listener(io_context&, _OnAccept&&,
             typename std::enable_if<base::traits::TryInvoke<
             _OnAccept, void(socket_type)>::value, int>::type = 0);

    template<class _OnAccept, class _OnError>
    explicit
    listener(io_context&, _OnAccept&&, _OnError&&,
             typename std::enable_if<base::traits::TryInvoke<
             _OnAccept, void(socket_type)>::value and
             base::traits::TryInvoke<_OnError, void(
                 boost::system::error_code, const char*)>::value, int>::type = 0);

private:

    self_type&
    loop(endpoint_type const& endpoint);

    void
    do_loop(boost::system::error_code ec = {});

    acceptor_type acceptor_;
    socket_type socket_;
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

#include <reactor/impl/listener.hxx>

#endif // not defined BEASTHTTP_REACTOR_LISTENER_HXX
