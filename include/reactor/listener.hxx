#if not defined BEASTHTTP_REACTOR_LISTENER_HXX
#define BEASTHTTP_REACTOR_LISTENER_HXX

#include "base/traits.hxx"

#include <boost/asio/io_context.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/coroutine.hpp>

#include <boost/asio/ip/tcp.hpp>

#define BEASTHTTP_LISTENER_TMPL_ATTRIBUTES \
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
class listener : public std::enable_shared_from_this<listener<BEASTHTTP_LISTENER_TMPL_ATTRIBUTES>>
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

    static_assert (base::traits::TryBind<acceptor_type, endpoint_type, boost::system::error_code&>::value,
                   "Invalid Endpoint type");

    static_assert (base::traits::TryInvoke<on_accept_type, socket_type>::value,
                   "Invalid OnAccept handler!");

    static_assert (base::traits::TryInvoke<on_error_type, boost::system::error_code, const char*>::value,
                   "Invalid OnError handler!");

    listener(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    template<class... _OnAction>
    static self_type&
    loop(io_context& ioc,
         endpoint_type const& endpoint,
         _OnAction&&... on_action)
    {
        return std::make_shared<self_type>
                (ioc, std::forward<_OnAction>(on_action)...)->loop(endpoint);
    }

    boost::system::error_code
    close()
    {
        auto ec = boost::system::error_code{};
        acceptor_.close(ec);

        if (ec and on_error_)
            on_error_(ec, "close");

        return ec;
    }

    endpoint_type const&
    endpoint() const
    {
        return endpoint_;
    }

    socket_type
    socket()
    {
        return std::move(socket_);
    }

    template<class _OnAccept>
    explicit
    listener(io_context& ioc,
             _OnAccept&& on_accept)
        : acceptor_{ioc},
          socket_{ioc},
          on_accept_{std::forward<_OnAccept>(on_accept)}
    {}

    template<class _OnAccept, class _OnError>
    explicit
    listener(io_context& ioc,
             _OnAccept&& on_accept,
             _OnError&& on_error)
        : acceptor_{ioc},
          socket_{ioc},
          on_accept_{std::forward<_OnAccept>(on_accept)},
          on_error_{std::forward<_OnError>(on_error)}
    {}

private:

    self_type&
    loop(endpoint_type const& endpoint)
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

    void
    do_loop(boost::system::error_code ec = {})
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

#endif // not defined BEASTHTTP_REACTOR_LISTENER_HXX
