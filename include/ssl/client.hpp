#ifndef BEAST_HTTP_SSL_CLIENT_HPP
#define BEAST_HTTP_SSL_CLIENT_HPP

#include "base.hpp"
#include "session.hpp"

namespace http{

namespace ssl {

/// @brief Class for communication with a remote host
/// @tparam Body type for response message
template<class ResBody>
class client_impl{

    using self_type = client_impl<ResBody>;
    using session_type = session<false, ResBody>;

    using on_connection_fn
            = std::function<void (session_type&)>;

    using on_handshake_fn
            = std::function<void (session_type&)>;

    using on_message_fn
            = std::function<void (boost::beast::http::response<ResBody>&,
                                  session_type&)>;
    using on_error_fn
            = std::function<void (boost::beast::error_code const &)>;

    void on_connect_(const boost::system::error_code & ec){
        if(ec){
            http::base::fail(ec, "connect");
            if(on_error)
                on_error(ec);

            return;
        }

        session_type::
                on_connect(connection_, on_connect, on_handshake, on_message, on_error);
    }

    bool process(std::string const & host, uint32_t port){
        auto ec = boost::system::error_code{};
        auto endpoint = processor_.resolve(host, port, ec);

        if(ec){
            if(on_error)
                on_error(ec);

            return false;
        }

        connection_.async_connect(endpoint,
                                  std::bind(&self_type::on_connect_, this, std::placeholders::_1));

        return true;
    }

    http::base::processor& processor_;
    base::connection connection_;

public:

    on_connection_fn on_connect;
    on_handshake_fn on_handshake;
    on_message_fn on_message;
    on_error_fn on_error;

    explicit client_impl(boost::asio::ssl::context & ctx)
        :  processor_{http::base::processor::get()},
          connection_{ctx, processor_.io_service()}
    {}

    bool invoke(std::string const & host, uint32_t port){
        return process(host, port);
    }

    template<class Callback1, class Callback2, class Callback3, class Callback4>
    bool invoke(std::string const & host, uint32_t port,
                Callback1&& on_connect_handler, Callback2&& on_handshake_handler,
                Callback3&& on_message_handler, Callback4&& on_error_handler){
        on_connect = std::forward<Callback1>(on_connect_handler);
        on_handshake = std::forward<Callback2>(on_handshake_handler);
        on_message = std::forward<Callback3>(on_message_handler);
        on_error = std::forward<Callback4>(on_error_handler);
        return process(host, port);
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace ssl

} // namespace http

#endif // BEAST_HTTP_SSL_CLIENT_HPP
