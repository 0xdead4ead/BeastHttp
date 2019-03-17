#if not defined BEASTHTTP_REACTOR_IMPL_LISTENER_HXX
#define BEASTHTTP_REACTOR_IMPL_LISTENER_HXX

#define BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE \
    template<template<typename> class OnAccept, \
    template<typename> class OnError, \
    class Protocol, \
    template<typename> class Acceptor, \
    template<typename> class Socket, \
    template<typename> class Endpoint>

namespace _0xdead4ead {
namespace http {
namespace reactor {

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class... _OnAction>
auto
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::loop(io_context& ioc,
     endpoint_type const& endpoint,
     _OnAction&&... on_action) -> decltype (
        self_type{std::declval<io_context&>(), std::declval<_OnAction>()...},
        std::declval<self_type&>())
{
    return std::make_shared<self_type>
            (ioc, std::forward<_OnAction>(on_action)...)->loop(endpoint);
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
boost::system::error_code
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::close()
{
    auto ec = boost::system::error_code{};
    acceptor_.close(ec);

    if (ec and on_error_)
        on_error_(ec, "close");

    return ec;
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
typename listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>
         ::endpoint_type const&
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::endpoint() const
{
    return endpoint_;
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
typename listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::socket_type
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::socket()
{
    return std::move(socket_);
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class _OnAccept>
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::listener(
        io_context& ioc, _OnAccept&& on_accept,
        typename std::enable_if<base::traits::TryInvoke<
        _OnAccept, void(socket_type)>::value, int>::type)
    : acceptor_{ioc},
      socket_{ioc},
      on_accept_{std::forward<_OnAccept>(on_accept)}
{
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class _OnAccept, class _OnError>
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::listener(
        io_context& ioc, _OnAccept&& on_accept, _OnError&& on_error,
        typename std::enable_if<base::traits::TryInvoke<
        _OnAccept, void(socket_type)>::value and
        base::traits::TryInvoke<_OnError, void(
            boost::system::error_code, const char*)>::value, int>::type)
    : acceptor_{ioc},
      socket_{ioc},
      on_accept_{std::forward<_OnAccept>(on_accept)},
      on_error_{std::forward<_OnError>(on_error)}
{
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
typename listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::self_type&
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::loop(
        endpoint_type const& endpoint)
{
    auto ec = boost::system::error_code{};
    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec and on_error_) {
        on_error_(ec, "open/loop");
        return *this;
    }

    // Allow address reuse
    acceptor_.set_option(boost::asio::socket_base::reuse_address(false));
    if (ec and on_error_) {
        on_error_(ec, "set_option/loop");
        return *this;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec and on_error_) {
        on_error_(ec, "bind/loop");
        return *this;
    }

    // Start listening for connections
    acceptor_.listen(
        boost::asio::socket_base::max_listen_connections, ec);
    if (ec and on_error_) {
        on_error_(ec, "listen/loop");
        return *this;
    }

    endpoint_ = endpoint;

    do_loop();
    return *this;
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
void
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::do_loop(
        boost::system::error_code ec)
{
    BOOST_ASIO_CORO_REENTER(*this){
        for(;;)
        {
            BOOST_ASIO_CORO_YIELD acceptor_.async_accept(
                        socket_,
                        std::bind(
                            &self_type::do_loop,
                            this->shared_from_this(),
                            std::placeholders::_1));
            if (ec and on_error_)
                on_error_(ec, "accept/do_loop");
            else
                on_accept_(std::move(socket_));
        }
    }
}

} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_IMPL_LISTENER_HXX
