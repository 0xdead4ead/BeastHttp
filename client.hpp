#ifndef CLIENT_HPP
#define CLIENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <session.hpp>

namespace http{

///
///
/// \brief Class for communication with a remote host
/// \tparam Body type for response message
template<class ResBody>
class client_impl{

public:

    client_impl() : status(false),
        connection_(nullptr),
        current_session_(nullptr),
        res_(boost::make_shared<boost::beast::http::response<ResBody> >()),
        res_handler_(nullptr)
    {}

    /// \brief Get request a resource
    /// \param Remote host (www.example.com, 127.0.0.1 and etc.)
    /// \param Session port (80, 8080)
    /// \param handler informing you of a successful connection
    /// Handler1 signature : template<class SessionPtr>
    ///                      void (const SessionPtr & session)
    /// \param handler on the received message
    /// Handler2 signature : template<class MessagePtr>
    ///                      void (const MessagePtr & message)
    template<class Handler1, class Handler2>
    void get(std::string const & host, uint32_t port, Handler1 && on_connect_handler, Handler2 && on_receive_handler){
        connection_ = base::processor::get().create_connection(host,
                                                               port,
                                                               [this, handler_ = boost::forward<Handler1>(on_connect_handler)](const boost::system::error_code & ec){
            if(ec)
                return base::fail(ec, "connect");

            session<false, ResBody>::on_connect(connection_, current_session_, res_handler_, res_, handler_);
        });

        if(connection_)
            res_handler_ = boost::make_shared<handler_cell_type>(boost::bind<void>(boost::forward<Handler2>(on_receive_handler),
                                                                                   boost::cref(res_)));
        else
            res_handler_ = {};
    }

private:

    bool status;
    base::tcp_connection::ptr connection_;
    typename session<false, ResBody>::ptr current_session_;
    boost::shared_ptr<boost::beast::http::response<ResBody> > res_;
    handler_cell_type::ptr res_handler_;

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace http

#endif // CLIENT_HPP
