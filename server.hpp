#ifndef SERVER_HPP
#define SERVER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <session.hpp>

namespace http {

///
///
/// \brief HTTP server class
/// \tparam Body type for request message
template<class ReqBody>
class server_impl{

public:

    server_impl() : status(false),
        current_session_(nullptr),
        req_(boost::make_shared<boost::beast::http::request<ReqBody> >()),
        def_handler_(nullptr),
        handler_storage_(boost::make_shared<std::map<method_type, boost::unordered_map<regex_type, handler_cell_type::ptr> > >())
    {}

    /// \brief Adds a handler for the requested resource
    /// \param Regular expression. Request content
    /// \param Method (GET, POST and etc.)
    /// \param Handler on receipt
    /// Handler signature : template<class MessagePtr, class SessionPtr>
    ///                     void (const MessagePtr & message, const SessionPtr & session)
    template<class Handler>
    void get(regex_type path_to_resource, method_type method, Handler && on_resource_handler){
        if(status)
            return http::base::fail("Adding a handler is not available when the server is running");

        handler_storage_->insert(std::make_pair(method,
                                               boost::unordered_map<regex_type, handler_cell_type::ptr>()
                                               ));

        auto & handler_map = handler_storage_->at(method);
        handler_map.insert(std::make_pair(path_to_resource,
                                          boost::make_shared<handler_cell_type>(boost::bind<void>(boost::forward<Handler>(on_resource_handler),
                                                                                            boost::cref(req_),
                                                                                            boost::cref(current_session_)))
                                          ));
    }

    /// \brief Adds default handler
    /// \note If the handler for the requested resource is not found, this on is called
    /// \param Handler on receipt
    /// Handler signature : template<class MessagePtr, class SessionPtr>
    ///                     void (const MessagePtr & message, const SessionPtr & session)
    template<class Handler>
    void def(Handler && default_handler){
        if(status)
            return http::base::fail("Adding a handler is not available when the server is running");

        def_handler_ = boost::make_shared<handler_cell_type>(boost::forward<Handler>(default_handler));
    }

    /// \brief Start accepting incoming connections
    /// \param Listening interface
    /// \param port
    /// \param Handler on new incoming connection
    /// Handler signature : template<class SessionPtr>
    ///                     void (const SessionPtr & session)
    template<class Handler>
    void listen(const std::string & address, uint32_t port, Handler && on_accept_handler){
        if(status)
            return http::base::fail("server is already running");

        base::processor::get().add_listener(address, port,
                                            boost::bind(&session<true, ReqBody>::template on_accept<Handler>,
                                                        boost::placeholders::_1,
                                                        boost::ref(current_session_),
                                                        def_handler_,
                                                        handler_storage_,
                                                        req_,
                                                        boost::forward<Handler>(on_accept_handler))
                                            )->accept_start();
        status = true;
    }

private:

    bool status;
    typename session<true, ReqBody>::ptr current_session_;
    boost::shared_ptr<boost::beast::http::request<ReqBody> > req_;
    handler_cell_type::ptr def_handler_;
    boost::shared_ptr<std::map<method_type, boost::unordered_map<regex_type, handler_cell_type::ptr> > > handler_storage_;

}; // server_impl class

using server = server_impl<boost::beast::http::string_body>;

} // namespace http

#endif // SERVER_HPP
