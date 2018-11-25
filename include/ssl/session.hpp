#ifndef BEAST_HTTP_SSL_SESSION_HPP
#define BEAST_HTTP_SSL_SESSION_HPP

#include "base.hpp"
#include "../list_cb.hpp"
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>

namespace http {

#ifndef BEAST_HTTP_INVOKER_CALLBACK_IS_DEFINE
#define BEAST_HTTP_INVOKER_CALLBACK_IS_DEFINE

class cb_invoker{

public:

    template<class Message, class Session, class List_cb>
    void invoke_cb(Message & message, Session & session, List_cb & l_cb){
        l_cb.reset();
        l_cb.exec(message, session);
    }

};

#endif // BEAST_HTTP_INVOKER_CALLBACK_IS_DEFINE

namespace ssl {

using resource_regex_t = std::string;
using resource_t = boost::beast::string_view;
using method_t = boost::beast::http::verb;
using s_method_t = boost::beast::string_view;

//###########################################################################

/// @brief session class. Handles an HTTPS server connection
/// @tparam Type of body request message
template<bool isServer, class Body>
class session : private cb_invoker,
        public std::enable_shared_from_this<session<true, Body> >
{

    using self_type = session<true, Body>;
    using list_cb_t = list_cb<boost::beast::http::request<Body>, self_type>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    using on_timer_fn
            = std::function<void(self_type&)>;

    using on_accept_fn
            = std::function<void(self_type&)>;

    using on_handshake_fn
            = std::function<void (self_type&)>;

    using on_error_fn
            = std::function<void (boost::beast::error_code const&,
                                  boost::beast::string_view const&)>;

    on_timer_fn on_timer_;
    const on_handshake_fn& on_handshake_;
    const on_error_fn& on_error_;

    bool handshake_ = false;
    bool close_ = false;
    http::base::timer::duration default_duration_ = std::chrono::milliseconds(500);

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
        bool is_full() const{
            return items_.size() >= limit;
        }

        // Called when a message finishes sending
        // Returns `true` if the caller should initiate a read
        bool on_write(){
            assert(! items_.empty());
            auto const was_full = is_full();
            items_.erase(items_.begin());
            if(! items_.empty())
                (*items_.front())();
            return was_full;
        }

        // Called by the HTTP handler to send a response.
        template<class Responce>
        void operator()(Responce && msg){
            // This holds a work item
            struct work_impl : work
            {
                session<true, Body>& self_;
                std::decay_t<Responce> msg_;

                work_impl(session<true, Body>& self, Responce&& msg)
                    : self_(self)
                    , msg_(std::forward<Responce>(msg))
                {}

                void operator()(){
                    self_.connection_.async_write(msg_,
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

    explicit session(boost::asio::ssl::context& ctx,
                     boost::asio::ip::tcp::socket&& socket,
                     const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                     const std::shared_ptr<method_map_t> & method_map_cb_p,
                     const on_handshake_fn & on_handshake,
                     const on_error_fn & on_error)
        : on_handshake_{on_handshake},
          on_error_{on_error},
          timer_{socket.get_executor(),
                 (std::chrono::steady_clock::time_point::max)()},
          connection_{ctx, std::move(socket)},
          resource_map_cb_p_{resource_map_cb_p},
          method_map_cb_p_{method_map_cb_p},
          queue_{*this}
    {}

    static void on_accept(boost::asio::ssl::context& ctx,
                          boost::asio::ip::tcp::socket&& socket,
                          const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                          const std::shared_ptr<method_map_t> & method_map_cb_p,
                          const on_accept_fn & on_accept,
                          const on_handshake_fn & on_handshake,
                          const on_error_fn & on_error){
        auto new_session_p = std::make_shared<self_type>(
                    ctx, std::move(socket), resource_map_cb_p, method_map_cb_p, on_handshake, on_error);
        if(on_accept)
            on_accept(*new_session_p);
    }

    auto & getConnection(){
        return connection_;
    }

    void do_handshake(){
        if(handshake_)
            return;

        timer_.stream().expires_after(default_duration_);

        connection_.async_handshake_server(buffer_,
                                       std::bind(&session<true, Body>::on_handshake,
                                                 this->shared_from_this(),
                                                 std::placeholders::_1,
                                                 std::placeholders::_2));
    }

    void do_read(){
        if(!handshake_)
            return;

        timer_.stream().expires_after(default_duration_);

        req_ = {};

        connection_.async_read(
                    buffer_,
                    req_,
                    std::bind(&session<true, Body>::on_read, this->shared_from_this(),
                              std::placeholders::_1,
                              std::placeholders::_2));
    }

    template<class Responce>
    void do_write(Responce && msg){
        if(!handshake_)
            return;

        queue_(std::forward<Responce>(msg));
    }

    void do_close(){
        close_ = true;

        // Is this connection alive?
        if(!connection_.stream().lowest_layer().is_open())
            return;

        // Set the timer
        timer_.stream().expires_after(std::chrono::seconds(10));

        // Perform the SSL shutdown
        connection_.async_shutdown(std::bind(
                                          &session<true, Body>::on_shutdown,
                                          this->shared_from_this(),
                                          std::placeholders::_1));
    }

    void set_expires_after(http::base::timer::duration duration
                                              = std::chrono::milliseconds(500)){
        default_duration_ = duration;
    }

    void launch_timer(){
        timer_.async_wait(
                    std::bind(
                        &session<true, Body>::on_timer,
                        this->shared_from_this(),
                        std::placeholders::_1));
    }

    template<class F>
    void launch_timer(F&& f){

        on_timer_ = std::forward<F>(f);

        timer_.async_wait(
                    std::bind(
                        &session<true, Body>::on_timer,
                        this->shared_from_this(),
                        std::placeholders::_1));
    }

protected:

    void on_timer(boost::system::error_code ec){
        if(ec && ec != boost::asio::error::operation_aborted){
            http::base::fail(ec, "timer");

            if(on_error_)
                on_error_(ec, "timer");

            return;
        }

        // Verify that the timer really expired since the deadline may have moved.
        if(timer_.stream().expiry() <= std::chrono::steady_clock::now())
        {
            // If this is true it means we timed out performing the shutdown
            if(close_)
                return;

            if(on_timer_ && connection_.stream().next_layer().is_open())
            {
                on_timer_(*this);

                return;
            }

            // Start the timer again
            timer_.stream().expires_at(
                (std::chrono::steady_clock::time_point::max)());
            launch_timer();
            do_close();
            return;
        }

        launch_timer();
    }

    void on_handshake(const boost::system::error_code & ec, std::size_t bytes_used){
        if(ec){
            http::base::fail(ec, "handshake");

            if(on_error_)
                on_error_(ec, "handshake");

            return;
        }

        handshake_ = true;

        if(on_handshake_)
            on_handshake_(*this);

        // Consume the portion of the buffer used by the handshake
        buffer_.consume(bytes_used);

        do_read();
    }

    void on_shutdown(const boost::system::error_code & ec){
        // Happens when the shutdown times out
        if(ec == boost::asio::error::operation_aborted)
            return;

        if(ec){
            http::base::fail(ec, "shutdown");

            if(on_error_)
                on_error_(ec, "shutdown");

            return;
        }
    }

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred){

        boost::ignore_unused(bytes_transferred);

        if(ec == boost::beast::http::error::end_of_stream)
            return do_close();

        if(ec){
            http::base::fail(ec, "read");

            if(on_error_)
                on_error_(ec, "read");

            return;
        }

        process_request();
    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred, bool close){
        boost::ignore_unused(bytes_transferred);

        if(ec){
            http::base::fail(ec, "write");

            if(on_error_)
                on_error_(ec, "write");

            return;
        }

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

    void process_request(){
        resource_t target = req_.target();
        method_t method = req_.method();

        bool invoked = false;
        if(method_map_cb_p_){
            auto method_pos = method_map_cb_p_->find(method);
            if(method_pos != method_map_cb_p_->end()){

                auto & resource_map = method_pos->second;

                for(const auto & value : resource_map){
                    const boost::regex e(value.first, boost::regex::perl | boost::regex::no_except);
                    if(boost::regex_match(target.to_string(), e)){
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
                if(boost::regex_match(target.to_string(), e)){
                    auto const & cb_p = value.second;

                    if(cb_p && !invoked)
                        invoke_cb(req_, *this, *cb_p);
                }
            }

        // If we aren't at the queue limit, try to pipeline another request
        if(! queue_.is_full() && connection_.stream().lowest_layer().is_open())
            do_read();
    }

    http::base::timer timer_;
    base::connection connection_;
    boost::beast::flat_buffer buffer_;
    const std::shared_ptr<resource_map_t> & resource_map_cb_p_;
    const std::shared_ptr<method_map_t> & method_map_cb_p_;
    boost::beast::http::request<Body> req_;
    queue queue_;

};

/// \brief session class. Handles an HTTPS client connection
/// \tparam Type of body response message
template<class Body>
class session<false, Body> : private cb_invoker,
        public std::enable_shared_from_this<session<false, Body> >{

    using self_type = session<false, Body>;

    using on_connection_fn
            = std::function<void (self_type&)>;

    using on_handshake_fn
            = std::function<void (self_type&)>;

    using on_message_fn
            = std::function<void (boost::beast::http::response<Body>&,
                                  self_type&)>;

    using on_error_fn
            = std::function<void (boost::beast::error_code const&,
                                  boost::beast::string_view const&)>;

    const on_handshake_fn& on_handshake_;
    const on_message_fn& on_message_;
    const on_error_fn& on_error_;

public:

    explicit session(base::connection & connection,
                     const on_handshake_fn & on_handshake,
                     const on_message_fn & on_message,
                     const on_error_fn & on_error)
        : connection_{connection},
          on_handshake_{on_handshake},
          on_message_{on_message},
          on_error_{on_error}
    {}

    static void on_connect(base::connection & connection,
                           const on_connection_fn & on_connect,
                           const on_handshake_fn & on_handshake,
                           const on_message_fn & on_message,
                           const on_error_fn & on_error){
        auto new_session_p = std::make_shared<session<false, Body>>(
                    connection, on_handshake, on_message, on_error);
        if(on_connect)
            on_connect(*new_session_p);

    }

    auto & getConnection(){
        return connection_;
    }

    void do_handshake(){
        if(handshake_)
            return;

        connection_.async_handshake_client(std::bind(
                                                  &session::on_handshake,
                                                  this->shared_from_this(),
                                                  std::placeholders::_1));
    }

    void do_read(){
        if(!handshake_)
            return;

        res_ = {};

        connection_.async_read(
                    buffer_,
                    res_,
                    std::bind(&session<false, Body>::on_read, this->shared_from_this(),
                                std::placeholders::_1,
                                std::placeholders::_2));
    }

    template<class Request>
    void do_write(Request && msg, bool next_read = true){
        if(!handshake_)
            return;

        auto sp = std::make_shared<std::decay_t<Request>>(std::forward<Request>(msg));
        msg_p_ = sp;

        connection_.async_write(*sp,
                                 std::bind(&session<false, Body>::on_write, this->shared_from_this(),
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           next_read
                                           ));
    }

    void do_close(){
        // Gracefully close the socket
        connection_.async_shutdown(std::bind(
                                          &session::on_shutdown,
                                          this->shared_from_this(),
                                          std::placeholders::_1));

    }

protected:

    void on_handshake(boost::system::error_code ec){
        if(ec){
            http::base::fail(ec, "handshake");

            if(on_error_)
                on_error_(ec, "handshake");

            return;
        }

        handshake_ = true;

        if(on_handshake_)
            on_handshake_(*this);
    }

    void on_shutdown(boost::system::error_code ec){
        if(ec == boost::asio::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec.assign(0, ec.category());
        }

        if(ec){
            http::base::fail(ec, "shutdown");

            if(on_error_)
                on_error_(ec, "shutdown");

            return;
        }

        // If we get here then the connection is closed gracefully
    }

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred){
        boost::ignore_unused(bytes_transferred);

        if(ec){
            http::base::fail(ec, "read");

            if(on_error_)
                on_error_(ec, "read");

            return;
        }

        if(on_message_)
            on_message_(res_, *this);

        // If we get here then the connection is closed gracefully
    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred, bool next_read){
        boost::ignore_unused(bytes_transferred);

        if(ec){
            http::base::fail(ec, "write");

            if(on_error_)
                on_error_(ec, "write");

            return;
        }

        if(next_read)
            do_read();
    }

    bool handshake_ = false;
    base::connection & connection_;
    boost::beast::http::response<Body> res_;
    std::shared_ptr<void> msg_p_;
    boost::beast::flat_buffer buffer_;

}; // class session

} // namespace ssl

} // namespace http

#endif // BEAST_HTTP_SSL_SESSION_HPP
