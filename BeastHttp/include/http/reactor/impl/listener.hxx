#if not defined BEASTHTTP_REACTOR_IMPL_LISTENER_HXX
#define BEASTHTTP_REACTOR_IMPL_LISTENER_HXX

#include <http/base/config.hxx>

#include <boost/asio/post.hpp>

#define BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE \
    template<template<typename> class OnAccept, \
    template<typename> class OnError, \
    class Protocol, \
    class Acceptor, \
    class Socket, \
    template<typename> class Endpoint>

namespace _0xdead4ead {
namespace http {
namespace reactor {

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class... _OnAction>
auto
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::launch(
        boost::asio::io_context& ctx, const endpoint_type& endpoint, _OnAction&&... on_action) -> decltype (
        void(self_type(std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)))
{
#if BEASTHTTP_USE_MAKE_SHARED == 0
    using Alloc = std::allocator<self_type>;

    Alloc a = Alloc();

    std::shared_ptr<self_type>(new (std::allocator_traits<Alloc>::allocate(a, 1)) self_type(
                ctx, std::forward<_OnAction>(on_action)...))->loop(endpoint);
#else
    std::make_shared<self_type>(ctx, std::forward<_OnAction>(on_action)...)->loop(endpoint);
#endif // BEASTHTTP_USE_MAKE_SHARED == 0
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class... _OnAction>
auto
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::launch(
        boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx,
     const endpoint_type& endpoint, _OnAction&&... on_action) -> decltype (
        void(self_type(std::declval<boost::asio::io_context&>(),
                       std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)))
{
#if BEASTHTTP_USE_MAKE_SHARED == 0
    using Alloc = std::allocator<self_type>;

    Alloc a = Alloc();

    std::shared_ptr<self_type>(new (std::allocator_traits<Alloc>::allocate(a, 1)) self_type(
                acceptor_ctx, socket_ctx, std::forward<_OnAction>(on_action)...))->loop(endpoint);
#else
    std::make_shared<self_type>(acceptor_ctx, socket_ctx, std::forward<_OnAction>(on_action)...)->loop(endpoint);
#endif // BEASTHTTP_USE_MAKE_SHARED == 0
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class Deleter, class Allocator, class... _OnAction>
auto
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::launch(
        boost::asio::io_context& ctx, const Deleter& d, const Allocator& alloc, const endpoint_type& endpoint,
        _OnAction&&... on_action) -> decltype (
        void(self_type(std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)))
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<self_type>;

    Alloc a = Alloc(alloc);

    std::shared_ptr<self_type>(new (std::allocator_traits<Alloc>::allocate(a, 1)) self_type(
                ctx, std::forward<_OnAction>(on_action)...), d, alloc)->loop(endpoint);
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class Deleter, class Allocator, class... _OnAction>
auto
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::launch(
        boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx,
        const Deleter& d, const Allocator& alloc, const endpoint_type& endpoint, _OnAction&&... on_action) -> decltype (
        void(self_type(std::declval<boost::asio::io_context&>(),
                       std::declval<boost::asio::io_context&>(), std::declval<_OnAction>()...)))
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<self_type>;

    Alloc a = Alloc(alloc);

    std::shared_ptr<self_type>(new (std::allocator_traits<Alloc>::allocate(a, 1)) self_type(
                acceptor_ctx, socket_ctx, std::forward<_OnAction>(on_action)...), d, alloc)
            ->loop(endpoint);
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class _OnAccept>
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::listener(
        boost::asio::io_context& ctx, _OnAccept&& on_accept,
        typename std::enable_if<base::traits::TryInvoke<
        _OnAccept, void(socket_type)>::value, int>::type)
    : base::strand_stream{ctx.get_executor()},
      acceptor_{ctx},
      socket_ctx_{ctx},
      on_accept_{std::forward<_OnAccept>(on_accept)}
{
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class _OnAccept, class _OnError>
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::listener(
        boost::asio::io_context& ctx, _OnAccept&& on_accept, _OnError&& on_error,
        typename std::enable_if<base::traits::TryInvoke<
        _OnAccept, void(socket_type)>::value and
        base::traits::TryInvoke<_OnError, void(
            boost::system::error_code, boost::string_view)>::value, int>::type)
    : base::strand_stream{ctx.get_executor()},
      acceptor_{ctx},
      socket_ctx_{ctx},
      on_accept_{std::forward<_OnAccept>(on_accept)},
      on_error_{std::forward<_OnError>(on_error)}
{
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class _OnAccept>
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::listener(
        boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx, _OnAccept&& on_accept,
        typename std::enable_if<base::traits::TryInvoke<
        _OnAccept, void(socket_type)>::value, int>::type)
    : base::strand_stream{acceptor_ctx.get_executor()},
      acceptor_{acceptor_ctx},
      socket_ctx_{socket_ctx},
      on_accept_{std::forward<_OnAccept>(on_accept)}
{
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
template<class _OnAccept, class _OnError>
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::listener(
        boost::asio::io_context& acceptor_ctx, boost::asio::io_context& socket_ctx, _OnAccept&& on_accept, _OnError&& on_error,
        typename std::enable_if<base::traits::TryInvoke<
        _OnAccept, void(socket_type)>::value and
        base::traits::TryInvoke<_OnError, void(
            boost::system::error_code, boost::string_view)>::value, int>::type)
    : base::strand_stream{acceptor_ctx.get_executor()},
      acceptor_{acceptor_ctx},
      socket_ctx_{socket_ctx},
      on_accept_{std::forward<_OnAccept>(on_accept)},
      on_error_{std::forward<_OnError>(on_error)}
{
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
void
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::on_spawn_connect(boost::system::error_code ec, socket_type& socket)
{
    if (ec) {
        if (on_error_)
            on_error_(ec, "accept/do_loop");

        return;
    }

    on_accept_(std::move(socket));
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
void
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::on_accept(boost::system::error_code ec, socket_type socket)
{
    boost::asio::post(static_cast<base::strand_stream&>(*this),
                      std::bind(&self_type::on_spawn_connect, this->shared_from_this(), ec, std::move(socket)));

    do_accept();
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
void
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::loop(
        const endpoint_type& endpoint)
{
    auto ec = boost::system::error_code{};

    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        if (on_error_)
            on_error_(ec, "open/loop");

        return;
    }
#ifdef _WIN32
    typedef boost::asio::detail::socket_option::boolean<BOOST_ASIO_OS_DEF(SOL_SOCKET), SO_EXCLUSIVEADDRUSE> exclusive_address_use;
    acceptor_.set_option(exclusive_address_use(true), ec);
#else
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
#endif
    if (ec) {
        if (on_error_)
            on_error_(ec, "set_option/loop");

        return;
    }

    acceptor_.bind(endpoint, ec);
    if (ec) {
        if (on_error_)
            on_error_(ec, "bind/loop");

        return;
    }

    acceptor_.listen(
        boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        if (on_error_)
            on_error_(ec, "listen/loop");

        return;
    }

    endpoint_ = endpoint;

    do_accept();
}

BEASTHTTP_REACTOR_LISTENER_TMPL_DECLARE
void
listener<BEASTHTTP_REACTOR_LISTENER_TMPL_ATTRIBUTES>::do_accept()
{
    acceptor_.async_accept(socket_ctx_,
                           std::bind(
                               &self_type::on_accept,
                               this->shared_from_this(),
                               std::placeholders::_1,
                               std::placeholders::_2));
}

} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_IMPL_LISTENER_HXX
