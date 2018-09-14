#ifndef BEAST_HTTP_SESSION_HPP
#define BEAST_HTTP_SESSION_HPP

#include "base.hpp"
#include "list_cb.hpp"
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>

namespace http {

using resource_regex_t = std::string;
using resource_t = boost::beast::string_view;
using method_t = boost::beast::http::verb;
using s_method_t = boost::beast::string_view;

class cb_invoker{

public:

    template<class Message, class Session, class List_cb>
    void invoke_cb(Message & message, Session & session, List_cb & l_cb){
        l_cb.reset();
        l_cb.exec(message, session);
    }

};

//###########################################################################

/// \brief session class. Handles an HTTP server connection
/// \tparam Type of body request message
template<bool isServer, class Body>
class session  : private cb_invoker, private boost::noncopyable,
        public std::enable_shared_from_this<session<true, Body> >
{

    using list_cb_t = list_cb<boost::beast::http::request<Body>, session<true, Body>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    std::function<void(session<true, Body>&)> on_timer_cb;

    //https://www.boost.org/doc/libs/1_68_0/libs/beast/example/advanced/server/advanced_server.cpp
    class queue{
        constexpr static size_t limit = 8;

        struct work
        {
            virtual ~work() = default;
            virtual void operator()() = 0;
        };

        session<true, Body>& self_;
        std::vector<std::unique_ptr<work>> items_;

    public:
        explicit queue(session<true, Body>& self)
            : self_(self)
        {
            static_assert(limit > 0, "queue limit must be positive");
            items_.reserve(limit);
        }

        // Returns `true` if we have reached the queue limit
        bool is_full() const
        {
            return items_.size() >= limit;
        }

        // Called when a message finishes sending
        // Returns `true` if the caller should initiate a read
        bool on_write()
        {
            assert(! items_.empty());
            auto const was_full = is_full();
            items_.erase(items_.begin());
            if(! items_.empty())
                (*items_.front())();
            return was_full;
        }

        // Called by the HTTP handler to send a response.
        template<class Responce>
        void operator()(Responce && msg)
        {
            // This holds a work item
            struct work_impl : work
            {
                session<true, Body>& self_;
                std::remove_reference_t<Responce> msg_;

                work_impl(session<true, Body>& self, Responce&& msg)
                    : self_(self)
                    , msg_(std::forward<Responce>(msg))
                {}

                void operator()()
                {
                    self_.connection_p_->async_write(msg_,
                                             std::bind(&session<true, Body>::on_write, self_.shared_from_this(),
                                                         std::placeholders::_1,
                                                         std::placeholders::_2,
                                                         msg_.need_eof()));
                }
            };

            // Allocate and store the work
            items_.push_back(std::make_unique<work_impl>(self_, std::forward<Responce>(msg)));

            // If there was no previous work, start this one
            if(items_.size() == 1)
                (*items_.front())();
        }
    };

public:

    using ReqBody = Body;

    explicit session(boost::asio::ip::tcp::socket&& socket,
                     const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                     const std::shared_ptr<method_map_t> & method_map_cb_p)
        : timer_p_{std::make_shared<http::base::timer>(socket.get_executor(),
                                                       (std::chrono::steady_clock::time_point::max)())},
          connection_p_{std::make_shared<base::connection>(std::move(socket))},
          resource_map_cb_p_{resource_map_cb_p},
          method_map_cb_p_{method_map_cb_p},
          queue_{*this}
    {}

    static void on_accept(boost::asio::ip::tcp::socket&& socket,
                          const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                          const std::shared_ptr<method_map_t> & method_map_cb_p,
                          const std::function<void(session<true, ReqBody>&)> & on_accept_cb)
    {
        auto new_session_p = std::make_shared<session<true, Body> >(std::move(socket), resource_map_cb_p, method_map_cb_p);
        if(on_accept_cb)
            on_accept_cb(*new_session_p);
    }

    auto & getConnection() const
    {
        return connection_p_;
    }

    void do_read(){

        timer_p_->stream().expires_after(std::chrono::seconds(10));

        req_ = {};

        connection_p_->async_read(
                    buffer_,
                    req_,
                    std::bind(&session<true, Body>::on_read, this->shared_from_this(),
                                std::placeholders::_1,
                                std::placeholders::_2));

    }

    template<class Responce>
    void do_write(Responce && msg)
    {
        queue_(std::forward<Responce>(msg));
    }

    void do_close()
    {
        connection_p_->shutdown();
    }

    void launch_timer()
    {
        timer_p_->async_wait(
                    std::bind(
                        &session<true, Body>::on_timer,
                        this->shared_from_this(),
                        std::placeholders::_1));
    }

    template<class F>
    void launch_timer(F&& f){

        on_timer_cb = std::forward<F>(f);

        timer_p_->async_wait(
                    std::bind(
                        &session<true, Body>::on_timer,
                        this->shared_from_this(),
                        std::placeholders::_1));
    }

protected:

    void on_timer(boost::system::error_code ec)
    {
        if(ec && ec != boost::asio::error::operation_aborted)
            return base::fail(ec, "timer");

        // Verify that the timer really expired since the deadline may have moved.
        if(timer_p_->stream().expiry() <= std::chrono::steady_clock::now())
        {

            if(on_timer_cb)
            {
                on_timer_cb(*this);
                return;
            }

            // Closing the socket cancels all outstanding operations. They
            // will complete with boost::asio::error::operation_aborted
            connection_p_->stream().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            connection_p_->stream().close(ec);
            return;
        }

        launch_timer();
    }

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {

        boost::ignore_unused(bytes_transferred);

        if(ec == boost::beast::http::error::end_of_stream)
            return do_close();

        if(ec)
            return base::fail(ec, "read");

        process_request();

    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred, bool close)
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

        // Inform the queue that a write completed
        if(queue_.on_write())
        {
            // Read another request
            do_read();
        }
    }

    void process_request()
    {

        resource_t target = req_.target();
        method_t method = req_.method();

        bool invoked = false;
        if(method_map_cb_p_){
            auto method_pos = method_map_cb_p_->find(method);
            if(method_pos != method_map_cb_p_->end()){

                auto & resource_map = method_pos->second;

                for(const auto & value : resource_map){
                    const boost::regex e(value.first, boost::regex::perl | boost::regex::no_except);
                    if(boost::regex_match(std::string(target.data(), target.size()), e)){
                        auto const & cb_p = value.second;

                        if(cb_p){
                            invoke_cb(req_, *this, *cb_p);
                            invoked = true;
                        }
                    }
                }
            }
        }

        if(resource_map_cb_p_)
            for(const auto & value : *resource_map_cb_p_){
                const boost::regex e(value.first, boost::regex::perl | boost::regex::no_except);
                if(boost::regex_match(std::string(target.data(), target.size()), e)){
                    auto const & cb_p = value.second;

                    if(cb_p && !invoked)
                        invoke_cb(req_, *this, *cb_p);
                }
            }

        // If we aren't at the queue limit, try to pipeline another request
        if(! queue_.is_full())
            do_read();

    }

    base::timer::ptr timer_p_;
    base::connection::ptr connection_p_;
    const std::shared_ptr<resource_map_t> & resource_map_cb_p_;
    const std::shared_ptr<method_map_t> & method_map_cb_p_;
    boost::beast::http::request<Body> req_;
    boost::beast::flat_buffer buffer_;
    queue queue_;

}; // class session


/// \brief session class. Handles an HTTP client connection
/// \tparam Type of body response message
template<class Body>
class session<false, Body>  : private cb_invoker, private boost::noncopyable,
        public std::enable_shared_from_this<session<false, Body> >{

public:

    explicit session(base::connection::ptr & connection_p,
                     const std::function<void (boost::beast::http::response<Body>&, session<false, Body>&)> & on_message_cb)
        : connection_p_{connection_p},
          on_message_cb_{on_message_cb}
    {}

    static void on_connect(base::connection::ptr & connection_p,
                           const std::function<void (session<false, Body>&)> & on_connect_cb,
                           const std::function<void (boost::beast::http::response<Body>&, session<false, Body>&)> & on_message_cb)
    {
        auto new_session_p = std::make_shared<session<false, Body>>(connection_p, on_message_cb);
        if(on_connect_cb)
            on_connect_cb(*new_session_p);
    }

    void do_read()
    {
        res_ = {};

        connection_p_->async_read(
                    buffer_,
                    res_,
                    std::bind(&session<false, Body>::on_read, this->shared_from_this(),
                                std::placeholders::_1,
                                std::placeholders::_2));
    }

    template<class Request>
    void do_write(Request && msg)
    {
        auto sp = std::make_shared<std::decay_t<Request>>(std::forward<Request>(msg));
        msg_p_ = sp;

        connection_p_->async_write(*sp,
                                 std::bind(&session<false, Body>::on_write, this->shared_from_this(),
                                             std::placeholders::_1,
                                             std::placeholders::_2
                                             ));
    }

    void do_close()
    {
        boost::system::error_code ec;
        // Gracefully close the socket
        connection_p_->stream().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes so don't bother reporting it.
        if(ec && ec != boost::system::errc::not_connected)
            return base::fail(ec, "shutdown");
    }

private:

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return base::fail(ec, "read");

        if(on_message_cb_)
            on_message_cb_(res_, *this);

        // If we get here then the connection is closed gracefully
    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return base::fail(ec, "write");

        do_read();
    }

    base::connection::ptr & connection_p_;
    const std::function<void (boost::beast::http::response<Body>&, session<false, Body>&)> & on_message_cb_;
    boost::beast::http::response<Body> res_;
    std::shared_ptr<void> msg_p_;
    boost::beast::flat_buffer buffer_;

}; // class session


} // namespace http

#endif // BEAST_HTTP_SESSION_HPP
