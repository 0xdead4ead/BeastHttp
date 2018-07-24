#ifndef SESSION_HPP
#define SESSION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/regex.hpp>

namespace http {

// type of resource
using regex_type = std::string;
using resource_type = boost::beast::string_view;
using method_type = boost::beast::http::verb;
using s_method_type = boost::beast::string_view;

/// \brief The handler_cell_type class represents components for the resource handler.
class handler_cell_type{

public:

    using handler_type = boost::function<void ()>;
    using ptr = boost::shared_ptr<handler_cell_type>;

    handler_cell_type(){}

    template<class Handler>
    handler_cell_type(Handler&& handler) :
        invoke_(boost::forward<Handler>(handler))
    {}

    handler_type invoke_;

}; // handler_cell_type class

//###########################################################################

/// \brief session class. Handles an HTTP server connection
/// \tparam Type of body message
template<bool isServer ,class Body>
class session  : public boost::enable_shared_from_this<session<true, Body> >
{

public:

    using ptr = boost::shared_ptr< session<true, Body> >;


    explicit session(){}

    explicit session(const base::tcp_connection::ptr & connection,
                     const handler_cell_type::ptr & def_handler,
                     const boost::shared_ptr<std::map<method_type, boost::unordered_map<regex_type, handler_cell_type::ptr> > > & handler_storage,
                     const boost::shared_ptr<boost::beast::http::request<Body> > & req)
        : connection_(connection), def_handler_(def_handler) ,handler_storage_(handler_storage), req_(req)
    {}

    template<class Handler>
    static void on_accept(const base::tcp_connection::ptr& connection,
                          ptr & cur_session,
                          const handler_cell_type::ptr & def_handler,
                          const boost::shared_ptr<std::map<method_type, boost::unordered_map<regex_type, handler_cell_type::ptr> > > & handler_storage,
                          const boost::shared_ptr<boost::beast::http::request<Body> > & req,
                          const Handler & handler)
    {
        cur_session.reset(new session<true, Body>(connection, def_handler, handler_storage, req));
        handler((ptr const &)cur_session);
    }

    void do_read(){
        *req_ = {};

        connection_->async_read(
                    buffer_,
                    *req_,
                    boost::bind(&session<true, Body>::on_read, this->shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
    }

    template<class Body2>
    void do_write(boost::beast::http::response<Body2> && msg){
        auto sp = boost::make_shared<boost::beast::http::response<Body2> >(boost::move(msg));
        msg_ = sp;

        connection_->async_write(*sp,
                                 boost::bind(&session<true, Body>::on_write, this->shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred,
                                             sp->need_eof()));
    }

private:

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred){
        boost::ignore_unused(bytes_transferred);

        if(ec == boost::beast::http::error::end_of_stream)
            return do_close();

        if(ec)
            return base::fail(ec, "read");

        process_request();
    }

    void
    on_write(const boost::system::error_code & ec, std::size_t bytes_transferred, bool close)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return base::fail(ec, "write");

        if(close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // We're done with the response so delete it
        msg_ = nullptr;

        // Read another request
        do_read();
    }

    void do_close()
    {
        connection_->shutdown();
    }

    void process_request()
    {
        auto method_pos = handler_storage_->find(req_->method());
        if(method_pos != handler_storage_->end()){
            resource_type target = req_->target();

            auto & handle_map = method_pos->second;

            for(auto & value : handle_map){
                const boost::regex e(value.first, boost::regex::perl | boost::regex::no_except);
                if(boost::regex_match(std::string(target.data(), target.size()), e)){
                    value.second->invoke_();
                    return;
                }
            }
        }

        if(def_handler_){
            def_handler_->invoke_();
            return;
        }

        return do_read();

    }



    base::tcp_connection::ptr  connection_;
    handler_cell_type::ptr  def_handler_;
    boost::shared_ptr<std::map<method_type, boost::unordered_map<regex_type, handler_cell_type::ptr> > > handler_storage_;
    boost::shared_ptr<boost::beast::http::request<Body> > req_;
    boost::shared_ptr<void> msg_;
    boost::beast::flat_buffer buffer_;

}; // class session

/// \brief session class. Handles an HTTP client connection
/// \tparam Type of body message
template<class Body>
class session<false, Body>  : public boost::enable_shared_from_this<session<false, Body> >{

public:

    using ptr = boost::shared_ptr< session<false, Body> >;


    explicit session(){}

    explicit session(const base::tcp_connection::ptr & connection,
                     const handler_cell_type::ptr & res_handler,
                     const boost::shared_ptr<boost::beast::http::response<Body> > & res)
        : connection_(connection), res_handler_(res_handler), res_(res)
    {}

    void do_read(){
        *res_ = {};

        connection_->async_read(
                    buffer_,
                    *res_,
                    boost::bind(&session<false, Body>::on_read, this->shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
    }

    template<class Handler>
    static void on_connect(const base::tcp_connection::ptr& connection,
                           ptr & cur_session,
                           const handler_cell_type::ptr & res_handler,
                           const boost::shared_ptr<boost::beast::http::response<Body> > & res,
                           const Handler & handler){
        cur_session.reset(new session<false, Body>(connection, res_handler, res));
        handler((ptr const &)cur_session);
    }

    template<class Body2>
    void do_write(boost::beast::http::request<Body2> && msg){
        auto sp = boost::make_shared<boost::beast::http::request<Body2> >(boost::move(msg));
        msg_ = sp;

        connection_->async_write(*sp,
                                 boost::bind(&session<false, Body>::on_write, this->shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred
                                             ));
    }

private:

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return base::fail(ec, "read");

        if(res_handler_)
            res_handler_->invoke_();

        // Gracefully close the socket
        connection_->socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes so don't bother reporting it.
        if(ec && ec != boost::system::errc::not_connected)
            return base::fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return base::fail(ec, "write");

        do_read();
    }

    base::tcp_connection::ptr  connection_;
    handler_cell_type::ptr  res_handler_;
    boost::shared_ptr<boost::beast::http::response<Body> > res_;
    boost::shared_ptr<void> msg_;
    boost::beast::flat_buffer buffer_;

}; // class session


} // namespace http

#endif // SESSION_HPP
