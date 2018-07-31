#ifndef CLIENT_HPP
#define CLIENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <session.hpp>

namespace http{

/// \brief Class for communication with a remote host
/// \tparam Body type for response message
template<class ResBody>
class client_impl{

public:

    using list_cb_t = list_cb<boost::beast::http::response<ResBody>, session<false, ResBody>>;

    explicit client_impl() : status{false},
        connection_p_{nullptr},
        response_cb_p_{nullptr}
    {}

    /// \brief Get request a resource
    /// \param Remote host (www.example.com, 127.0.0.1 and etc.)
    /// \param Session port (80, 8080)
    /// \param handler informing you of a successful connection
    /// Callback1 signature : template<class Session>
    ///                      void (const Session & session)
    /// \param handler on the received message
    /// Callback2 signature : template<class Message>
    ///                      void (const Message & message, Session & session)
    template<class Callback1, class Callback2>
    void invoke(std::string const & host, uint32_t port, Callback1 && on_connect_handler, Callback2 && on_receive_handler){
        connection_p_ = base::processor::get().create_connection(host,
                                                                 port,
                                                                 [this, handler_ = boost::forward<Callback1>(on_connect_handler)](const boost::system::error_code & ec){
            if(ec)
                return base::fail(ec, "connect");

            session<false, ResBody>::on_connect(connection_p_, response_cb_p_, handler_);
        });

        typename list_cb_t::L cb{
            boost::bind<void>(
                        boost::forward<Callback2>(on_receive_handler),
                        boost::placeholders::_1,
                        boost::placeholders::_2
                        )
        };

        if(connection_p_)
            response_cb_p_ = boost::make_shared<list_cb_t>(cb);
        else
            response_cb_p_ = {};
    }

private:

    bool status;
    base::tcp_connection::ptr connection_p_;
    typename list_cb_t::ptr response_cb_p_;

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace http

#endif // CLIENT_HPP
