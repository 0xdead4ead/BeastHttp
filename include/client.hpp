#ifndef BEAST_HTTP_CLIENT_HPP
#define BEAST_HTTP_CLIENT_HPP

#include "base.hpp"
#include "session.hpp"

namespace http{

/// \brief Class for communication with a remote host
/// \tparam Body type for response message
template<class ResBody>
class client_impl{

    void process(std::string const & host, uint32_t port){
        connection_p_ = base::processor::get()
                .create_connection<base::connection>(host,
                                                     port,
                                                     [this](const boost::system::error_code & ec){
            if(ec){
                connection_p_->stream().get_executor().context().stop();
                return base::fail(ec, "connect");
            }

            session<false, ResBody>::on_connect(std::ref(connection_p_), std::cref(on_connect), std::cref(on_message));
        });

        if(!connection_p_){
            on_connect = {};
            on_message = {};
            base::processor::get().stop();
        }
    }

    base::connection::ptr connection_p_;

public:

    std::function<void (session<false, ResBody>&)> on_connect;
    std::function<void (boost::beast::http::response<ResBody>&, session<false, ResBody>&)> on_message;

    explicit client_impl()
        :  connection_p_{nullptr}
    {}

    void invoke(std::string const & host, uint32_t port){
        process(host, port);
    }
    /// \brief Get request a resource
    /// \param Remote host (www.example.com, 127.0.0.1 and etc.)
    /// \param Session port (80, 8080)
    /// \param handler informing you of a successful connection
    /// Callback1 signature : template<class Session>
    ///                      void (Session & session)
    /// \param handler on the received message
    /// Callback2 signature : template<class Message>
    ///                      void (Message & message, Session & session)
    template<class Callback1, class Callback2>
    void invoke(std::string const & host, uint32_t port, Callback1 && on_connect_handler, Callback2 && on_message_handler){

        on_connect = std::forward<Callback1>(on_connect_handler);
        on_message = std::forward<Callback2>(on_message_handler);

        process(host, port);
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace http

#endif // BEAST_HTTP_CLIENT_HPP
