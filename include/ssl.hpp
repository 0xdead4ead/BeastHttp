#ifndef BEAST_HTTP_SSL_HPP
#define BEAST_HTTP_SSL_HPP

#include <boost/asio/ssl/stream.hpp>
#include "base.hpp"
#include "session.hpp"
#include "list_cb.hpp"
#include "router.hpp"

namespace http{

namespace ssl{

namespace base{

/// \brief The ssl connection class
class connection : public http::base::connection_base<connection> {

    using base_t = http::base::connection_base<connection>;

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> stream_;

public:

    using ptr = std::shared_ptr<connection>;

    // Constructor for server to client connection
    explicit connection(boost::asio::ssl::context& ctx, boost::asio::ip::tcp::socket&& socket)
        : base_t{socket.get_executor()},
          socket_{std::move(socket)},
          stream_{socket_, ctx}
    {}

    // Constructor for client to server connection
    template<class F>
    explicit connection(
            boost::asio::ssl::context& ctx,
            boost::asio::io_service& ios,
            const boost::asio::ip::tcp::endpoint& endpoint, F&& f)
        : base_t{ios.get_executor()},
          socket_{ios},
          stream_{socket_, ctx}
    {
        socket_.async_connect(endpoint, std::forward<F>(f));
    }

    // Constructor for client to server connection (sync)
    explicit connection(
            boost::asio::ssl::context& ctx,
            boost::asio::io_service& ios,
            const boost::asio::ip::tcp::endpoint& endpoint,
            boost::system::error_code& ec)
        : base_t{ios.get_executor()},
          socket_{ios},
          stream_{socket_, ctx}
    {
        socket_.connect(endpoint, ec);

        if(ec)
            http::base::fail(ec, "connect");
    }

    template <class F, class B>
    void async_handshake_server(const B& buf, F&& f){
        stream_.async_handshake(
            boost::asio::ssl::stream_base::server, buf.data(),
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    template <class F>
    void async_handshake_client(F&& f){
        stream_.async_handshake(
            boost::asio::ssl::stream_base::client,
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    template<class B>
    auto handshake_server(const B& buf){
        boost::beast::error_code ec;
        stream_.handshake(boost::asio::ssl::stream_base::server, buf.data(), ec);

        if(ec)
            http::base::fail(ec, "handshake");

        return ec;
    }

    auto handshake_client(){
        boost::beast::error_code ec;
        stream_.handshake(boost::asio::ssl::stream_base::client, ec);

        if(ec)
            http::base::fail(ec, "handshake");

        return ec;
    }

    template<class F>
    void async_shutdown(F&& f){
        stream_.async_shutdown(
                    boost::asio::bind_executor(
                        strand_, std::forward<F>(f)));
    }

    auto shutdown(){
        boost::beast::error_code ec;
        stream_.shutdown(ec);

        if(ec)
            http::base::fail(ec, "shutdown");

        return ec;
    }

    auto & stream(){
        return stream_;
    }

    auto release_stream(){
        return std::move(socket_);
    }

};

} // namespace base

// synchronous calls

auto handshake_server(const base::connection::ptr & connection_p){
    boost::beast::multi_buffer buffer;
    return connection_p->handshake_server(buffer);
}

auto handshake_client(const base::connection::ptr & connection_p){
    return connection_p->handshake_client();
}

template<class Body>
auto send_request(const base::connection::ptr & connection_p,
                  /*const*/ boost::beast::http::request<Body> & msg){
    return connection_p->write(msg);
}

template<class Body>
auto send_responce(const base::connection::ptr & connection_p,
                   /*const*/ boost::beast::http::response<Body> & msg){
    return connection_p->write(msg);
}

template<class Body>
auto recv_request(const base::connection::ptr & connection_p,
                  boost::beast::http::request<Body> & msg){
    boost::beast::multi_buffer buffer;
    return connection_p->read(buffer, msg);
}

template<class Body>
auto recv_responce(const base::connection::ptr & connection_p,
                   boost::beast::http::response<Body> & msg){
    boost::beast::multi_buffer buffer;
    return connection_p->read(buffer, msg);
}
////

/// \brief session class. Handles an HTTPS server connection
/// \tparam Type of body request message
template<bool isServer, class Body>
class session  : private cb_invoker, private boost::noncopyable,
        public std::enable_shared_from_this<session<true, Body> >
{

    using list_cb_t = list_cb<boost::beast::http::request<Body>, session<true, Body>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    bool handshake_ = false;
    bool close_ = false;
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
                std::decay_t<Responce> msg_;

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

    explicit session(boost::asio::ssl::context& ctx,
                     boost::asio::ip::tcp::socket&& socket,
                     const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                     const std::shared_ptr<method_map_t> & method_map_cb_p)
        : timer_p_{std::make_shared<http::base::timer>(socket.get_executor(),
                                                       (std::chrono::steady_clock::duration::max)())},
          connection_p_{std::make_shared<base::connection>(ctx, std::move(socket))},
          resource_map_cb_p_{resource_map_cb_p},
          method_map_cb_p_{method_map_cb_p},
          queue_{*this}
    {}

    static void on_accept(boost::asio::ssl::context& ctx,
                          boost::asio::ip::tcp::socket&& socket,
                          const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                          const std::shared_ptr<method_map_t> & method_map_cb_p,
                          const std::function<void(session<true, Body>&)> & on_accept_cb)
    {
        auto new_session_p = std::make_shared<session<true, Body> >(ctx, std::move(socket), resource_map_cb_p, method_map_cb_p);
        if(on_accept_cb)
            on_accept_cb(*new_session_p);
    }

    auto & getConnection() const
    {
        return connection_p_;
    }

    void do_handshake(){
        if(handshake_)
            return;

        timer_p_->stream().expires_after(std::chrono::seconds(10));

        connection_p_->async_handshake_server(buffer_,
                                       std::bind(&session<true, Body>::on_handshake,
                                                 this->shared_from_this(),
                                                 std::placeholders::_1,
                                                 std::placeholders::_2));
    }

    void do_read(){
        if(!handshake_)
            return;

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
    void do_write(Responce && msg){
        if(!handshake_)
            return;

        queue_(std::forward<Responce>(msg));
    }

    void do_close(){
        close_ = true;

        // Is this connection alive?
        if(!connection_p_->stream().lowest_layer().is_open())
            return;

        // Set the timer
        timer_p_->stream().expires_after(std::chrono::seconds(10));

        // Perform the SSL shutdown
        connection_p_->async_shutdown(std::bind(
                                          &session<true, Body>::on_shutdown,
                                          this->shared_from_this(),
                                          std::placeholders::_1));
    }

    void launch_timer(){
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
            return http::base::fail(ec, "timer");

        // Verify that the timer really expired since the deadline may have moved.
        if(timer_p_->stream().expiry() <= std::chrono::steady_clock::now())
        {
            // If this is true it means we timed out performing the shutdown
            if(close_)
                return;

            if(on_timer_cb)
            {
                on_timer_cb(*this);
                return;
            }

            // Start the timer again
            timer_p_->stream().expires_at(
                (std::chrono::steady_clock::time_point::max)());
            launch_timer();
            do_close();
            return;
        }

        launch_timer();
    }

    void on_handshake(const boost::system::error_code & ec, std::size_t bytes_used)
    {
        if(ec)
            return http::base::fail(ec, "handshake");

        handshake_ = true;

        // Consume the portion of the buffer used by the handshake
        buffer_.consume(bytes_used);

        do_read();
    }

    void on_shutdown(const boost::system::error_code & ec)
    {
        // Happens when the shutdown times out
        if(ec == boost::asio::error::operation_aborted)
            return;

        if(ec)
            return http::base::fail(ec, "shutdown");
    }

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred){

        boost::ignore_unused(bytes_transferred);

        if(ec == boost::beast::http::error::end_of_stream)
            return do_close();

        if(ec)
            return http::base::fail(ec, "read");

        process_request();
    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred, bool close)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return http::base::fail(ec, "write");

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
        if(! queue_.is_full() && connection_p_->stream().lowest_layer().is_open())
            do_read();

    }

    http::base::timer::ptr timer_p_;
    base::connection::ptr connection_p_;
    boost::beast::flat_buffer buffer_;
    const std::shared_ptr<resource_map_t> & resource_map_cb_p_;
    const std::shared_ptr<method_map_t> & method_map_cb_p_;
    boost::beast::http::request<Body> req_;
    queue queue_;

};

/// \brief session class. Handles an HTTPS client connection
/// \tparam Type of body response message
template<class Body>
class session<false, Body>  : private cb_invoker, private boost::noncopyable,
        public std::enable_shared_from_this<session<false, Body> >{

public:

    explicit session(base::connection::ptr & connection_p,
                     const std::function<void (session<false, Body>&)> & on_handshake_cb,
                     const std::function<void (boost::beast::http::response<Body>&, session<false, Body>&)> & on_message_cb)
        : connection_p_{connection_p},
          on_handshake_cb_{on_handshake_cb},
          on_message_cb_{on_message_cb}

    {}

    static void on_connect(base::connection::ptr & connection_p,
                           const std::function<void (session<false, Body>&)> & on_connect_cb,
                           const std::function<void (session<false, Body> &)> & on_handshake_cb,
                           const std::function<void (boost::beast::http::response<Body>&, session<false, Body>&)> & on_message_cb){
        auto new_session_p = std::make_shared<session<false, Body>>(connection_p, on_handshake_cb, on_message_cb);
        if(on_connect_cb)
            on_connect_cb(*new_session_p);

    }

    auto & getConnection() const
    {
        return connection_p_;
    }

    void do_handshake(){
        if(handshake)
            return;

        connection_p_->async_handshake_client(std::bind(
                                                  &session::on_handshake,
                                                  this->shared_from_this(),
                                                  std::placeholders::_1));
    }

    void do_read(){
        if(!handshake)
            return;

        res_ = {};

        connection_p_->async_read(
                    buffer_,
                    res_,
                    std::bind(&session<false, Body>::on_read, this->shared_from_this(),
                                std::placeholders::_1,
                                std::placeholders::_2));
    }

    template<class Request>
    void do_write(Request && msg, bool next_read = true){
        if(!handshake)
            return;

        auto sp = std::make_shared<std::decay_t<Request>>(std::forward<Request>(msg));
        msg_p_ = sp;

        connection_p_->async_write(*sp,
                                 std::bind(&session<false, Body>::on_write, this->shared_from_this(),
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           next_read
                                           ));
    }

    void do_close(){
        // Gracefully close the socket
        connection_p_->async_shutdown(std::bind(
                                          &session::on_shutdown,
                                          this->shared_from_this(),
                                          std::placeholders::_1));

    }

protected:

    void on_handshake(boost::system::error_code ec)
    {
        if(ec)
            return http::base::fail(ec, "handshake");

        handshake = true;

        if(on_handshake_cb_)
            on_handshake_cb_(*this);
    }

    void on_shutdown(boost::system::error_code ec)
    {
        if(ec == boost::asio::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec.assign(0, ec.category());
        }
        if(ec)
            return http::base::fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }

    void on_read(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return http::base::fail(ec, "read");

        if(on_message_cb_)
            on_message_cb_(res_, *this);

        // If we get here then the connection is closed gracefully
    }

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred, bool next_read)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return http::base::fail(ec, "write");

        if(next_read)
            do_read();
    }

    bool handshake = false;
    base::connection::ptr & connection_p_;
    const std::function<void (session<false, Body> &)> & on_handshake_cb_;
    const std::function<void (boost::beast::http::response<Body>&, session<false, Body>&)> & on_message_cb_;
    boost::beast::http::response<Body> res_;
    std::shared_ptr<void> msg_p_;
    boost::beast::flat_buffer buffer_;

}; // class session

#ifdef BEAST_HTTP_SERVER_HPP

/// \brief HTTPS server class
/// \tparam Body type for request message
template <class ReqBody>
class server_impl : private http::server_impl<ReqBody>{

    using basic_r = basic_router<session<true, ReqBody>>;
    using chain_r = chain_router<session<true, ReqBody>>;
    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    std::shared_ptr<basic_r> basic_router_;
    std::shared_ptr<chain_r> chain_router_;
    std::shared_ptr<resource_map_t> resource_map_cb_p_;
    std::shared_ptr<method_map_t> method_map_cb_p_;

    const std::shared_ptr<basic_r> & get_basic_router(){
        if(!basic_router_)
            basic_router_ = std::make_shared<basic_r>(resource_map_cb_p_, method_map_cb_p_);

        return basic_router_;
    }

    const std::shared_ptr<chain_r> & get_chain_router(){
        if(!chain_router_)
            chain_router_ = std::make_shared<chain_r>(resource_map_cb_p_, method_map_cb_p_);

        return chain_router_;
    }

    void process(const std::string & address, uint32_t port){
        http::base::processor::get().add_listener(address, port,
                                            std::bind(&session<true, ReqBody>::on_accept,
                                                      std::ref(ctx_),
                                                      std::placeholders::_1,
                                                      std::cref(resource_map_cb_p_),
                                                      std::cref(method_map_cb_p_),
                                                      std::cref(on_accept)
                                                      )
                                            )->run();
    }

protected:

    boost::asio::ssl::context & ctx_;

public:

    std::function<void(session<true, ReqBody>&)> on_accept;

    // new ssl context
    explicit server_impl(boost::asio::ssl::context & ctx) :
        ctx_{ctx},
        basic_router_{nullptr},
        chain_router_{nullptr},
        resource_map_cb_p_{nullptr},
        method_map_cb_p_{nullptr}
    {}

    typename chain_r::ref route(const resource_regex_t & path_to_resource){
        return get_chain_router()->save_to_res_regex(path_to_resource).operator *();
    }
    /// Callback signature : template<class Message, class Session /*, class Next (optional)*/>
    ///                     void (Message & message, Session & session /*, Next & next (optional)*/)
    /// \brief Adds a handler for GET method
    template<class... Callback>
    void get(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->get(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for POST method
    template<class... Callback>
    void post(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->post(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PUT method
    template<class... Callback>
    void put(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->put(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for HEAD method
    template<class... Callback>
    void head(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->head(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for DELETE method
    template<class... Callback>
    void delete_(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->delete_(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for OPTIONS method
    template<class... Callback>
    void options(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->options(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for CONNECT method
    template<class... Callback>
    void connect(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->connect(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for TRACE method
    template<class... Callback>
    void trace(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->trace(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for COPY method
    template<class... Callback>
    void copy(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->copy(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for LOCK method
    template<class... Callback>
    void lock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->lock(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MKCOL method
    template<class... Callback>
    void mkcol(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->mkcol(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MOVE method
    template<class... Callback>
    void move(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->move(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PROPFIND method
    template<class... Callback>
    void propfind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->propfind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PROPPATCH method
    template<class... Callback>
    void proppatch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->proppatch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for SEARCH method
    template<class... Callback>
    void search(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->search(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNLOCK method
    template<class... Callback>
    void unlock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->unlock(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for BIND method
    template<class... Callback>
    void bind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->bind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for REBIND method
    template<class... Callback>
    void rebind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->rebind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNBIND method
    template<class... Callback>
    void unbind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->unbind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for ACL method
    template<class... Callback>
    void acl(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->acl(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // subversion
    /// \brief Adds a handler for REPORT method
    template<class... Callback>
    void report(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->report(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MKACTIVITY method
    template<class... Callback>
    void mkactivity(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->mkactivity(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for CHECKOUT method
    template<class... Callback>
    void checkout(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->checkout(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MERGE method
    template<class... Callback>
    void merge(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->merge(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // upnp
    /// \brief Adds a handler for MSEARCH method
    template<class... Callback>
    void msearch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->msearch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for NOTIFY method
    template<class... Callback>
    void notify(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->notify(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for SUBSCRIBE method
    template<class... Callback>
    void subscribe(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->subscribe(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNSUBSCRIBE method
    template<class... Callback>
    void unsubscribe(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->unsubscribe(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // RFC-5789
    /// \brief Adds a handler for PATCH method
    template<class... Callback>
    void patch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->patch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PURGE method
    template<class... Callback>
    void purge(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->purge(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // CalDAV
    /// \brief Adds a handler for MKCALENDAR method
    template<class... Callback>
    void mkcalendar(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->mkcalendar(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // RFC-2068, section 19.6.1.2
    /// \brief Adds a handler for LINK method
    template<class... Callback>
    void link(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->link(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNLINK method
    template<class... Callback>
    void unlink(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->unlink(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for the requested resource by default
    /// \note If the handler for the requested resource with method is not found, this on is called
    template<class... Callback>
    void all(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        get_basic_router()->all(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }

    template<class... Types>
    auto param(){
        return get_basic_router()->template param<Types...>();
    }

    void listen(const std::string & address, uint32_t port){
        process(address, port);
    }
    /// \brief Start accepting incoming connections
    /// \param Listening interface
    /// \param port
    /// \param Handler on new incoming connection
    /// Callback signature : template<class Session>
    ///                     void (Session & session)
    template<class Callback>
    void listen(const std::string & address, uint32_t port, Callback && on_accept_handler){
        on_accept = std::forward<Callback>(on_accept_handler);
        process(address, port);
    }


    basic_r BasicRouter() noexcept{
        return {};
    }

    chain_r ChainRouter() noexcept{
        return {};
    }

    void use(const resource_regex_t & path_to_resource, typename basic_r::cref other){
        get_basic_router()->use(path_to_resource, other);
    }

    void use(const resource_regex_t & path_to_resource, typename chain_r::cref other){
        get_chain_router()->use(path_to_resource, other);
    }

    void use(typename basic_r::cref other){
        get_basic_router()->use("", other);
    }

    void use(typename chain_r::cref other){
        get_chain_router()->use("", other);
    }

};

using server = server_impl<boost::beast::http::string_body>;

// the list wrapped methods (string literal only)
namespace literal{

template<class, class>
struct wrap_param;

struct wrap_get{ // GET

    const char * s_;
    std::size_t n_;

    wrap_get(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... fs) &&{
        instance.get({s_, n_}, std::forward<Callback>(fs)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_get, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_post{ // POST

    const char * s_;
    std::size_t n_;

    wrap_post(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.post({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_post, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_put{ // PUT

    const char * s_;
    std::size_t n_;

    wrap_put(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.put({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_put, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_head{ // HEAD

    const char * s_;
    std::size_t n_;

    wrap_head(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.head({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_head, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_delete_{ // DELETE

    const char * s_;
    std::size_t n_;

    wrap_delete_(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.delete_({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_delete_, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_options{ // OPTIONS

    const char * s_;
    std::size_t n_;

    wrap_options(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.options({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_options, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_connect{ // CONNECT

    const char * s_;
    std::size_t n_;

    wrap_connect(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.connect({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_connect, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_trace{ // TRACE

    const char * s_;
    std::size_t n_;

    wrap_trace(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.trace({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_trace, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_copy{ // COPY

    const char * s_;
    std::size_t n_;

    wrap_copy(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.copy({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_copy, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_lock{ // LOCK

    const char * s_;
    std::size_t n_;

    wrap_lock(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.lock({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_lock, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_mkcol{ // MKCOL

    const char * s_;
    std::size_t n_;

    wrap_mkcol(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.mkcol({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_mkcol, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_move{ // MOVE

    const char * s_;
    std::size_t n_;

    wrap_move(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.move({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_move, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_propfind{ // PROPFIND

    const char * s_;
    std::size_t n_;

    wrap_propfind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.propfind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_propfind, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_proppatch{ // PROPPATCH

    const char * s_;
    std::size_t n_;

    wrap_proppatch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.proppatch({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_proppatch, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_search{ // SEARCH

    const char * s_;
    std::size_t n_;

    wrap_search(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.search({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_search, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_unlock{ // UNLOCK

    const char * s_;
    std::size_t n_;

    wrap_unlock(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.unlock({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_unlock, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_bind{ // BIND

    const char * s_;
    std::size_t n_;

    wrap_bind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.bind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_bind, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_rebind{ // REBIND

    const char * s_;
    std::size_t n_;

    wrap_rebind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.rebind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_rebind, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_unbind{ // UNBIND

    const char * s_;
    std::size_t n_;

    wrap_unbind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.unbind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_unbind, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_acl{ // ACL

    const char * s_;
    std::size_t n_;

    wrap_acl(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.acl({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_acl, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_report{ // REPORT

    const char * s_;
    std::size_t n_;

    wrap_report(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.report({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_report, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_mkactivity{ // MKACTIVITY

    const char * s_;
    std::size_t n_;

    wrap_mkactivity(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.mkactivity({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_mkactivity, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_checkout{ // CHECKOUT

    const char * s_;
    std::size_t n_;

    wrap_checkout(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.checkout({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_checkout, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_merge{ // MERGE

    const char * s_;
    std::size_t n_;

    wrap_merge(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.merge({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_merge, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_msearch{ // MSEARCH

    const char * s_;
    std::size_t n_;

    wrap_msearch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.msearch({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_msearch, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_notify{ // NOTIFY

    const char * s_;
    std::size_t n_;

    wrap_notify(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.notify({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_notify, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_subscribe{ // SUBSCRIBE

    const char * s_;
    std::size_t n_;

    wrap_subscribe(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.subscribe({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_subscribe, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_unsubscribe{ // UNSUBSCRIBE

    const char * s_;
    std::size_t n_;

    wrap_unsubscribe(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.options({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_unsubscribe, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_patch{ // PATCH

    const char * s_;
    std::size_t n_;

    wrap_patch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.patch({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_patch, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_purge{ // PURGE

    const char * s_;
    std::size_t n_;

    wrap_purge(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.purge({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_purge, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_mkcalendar{ // MKCALENDAR

    const char * s_;
    std::size_t n_;

    wrap_mkcalendar(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.mkcalendar({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_mkcalendar, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_link{ // LINK

    const char * s_;
    std::size_t n_;

    wrap_link(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.link({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_link, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_unlink{ // UNLINK

    const char * s_;
    std::size_t n_;

    wrap_unlink(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.unlink({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_unlink, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

struct wrap_all{ // ALL

    const char * s_;
    std::size_t n_;

    wrap_all(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void assign(server_impl<ReqBody> & instance, Callback&&... f){
        instance.all({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Session, class Router, class... Types>
    auto assign(param_impl<Session, Router, Types...> && instance) &&{
        return wrap_param<wrap_all, param_impl<Session, Router, Types...>>{*this, std::move(instance)};
    }

};

template<class Wrap_Method, class Param>
struct wrap_param{ // Wrapped class for param implementation

    std::add_lvalue_reference_t<Wrap_Method> wrap_method_;
    std::add_rvalue_reference_t<Param> instance_;

    wrap_param(Wrap_Method & wrap_method, Param && instance)
        : wrap_method_{wrap_method}, instance_{std::move(instance)}
    {}

    template<class... Callback>
    std::enable_if_t<std::is_same<Wrap_Method, wrap_get>::value, void> with(Callback && ... fs) &&{
        static_cast<Param&&>(instance_).get({wrap_method_.s_, wrap_method_.n_}, std::forward<Callback>(fs)...);
    }

};

/// \param s - pointer to resource regex
/// \param n - length of string

inline auto operator "" _get(const char* s, std::size_t n)
{
    return wrap_get{s, n};
}

inline auto operator "" _post(const char* s, std::size_t n)
{
    return wrap_post{s, n};
}

inline auto operator "" _put(const char* s, std::size_t n)
{
    return wrap_put{s, n};
}

inline auto operator "" _head(const char* s, std::size_t n)
{
    return wrap_put{s, n};
}

inline auto operator "" _delete_(const char* s, std::size_t n)
{
    return wrap_delete_{s, n};
}

inline auto operator "" _options(const char* s, std::size_t n)
{
    return wrap_options{s, n};
}

inline auto operator "" _connect(const char* s, std::size_t n)
{
    return wrap_connect{s, n};
}

inline auto operator "" _trace(const char* s, std::size_t n)
{
    return wrap_trace{s, n};
}

inline auto operator "" _copy(const char* s, std::size_t n)
{
    return wrap_copy{s, n};
}

inline auto operator "" _lock(const char* s, std::size_t n)
{
    return wrap_lock{s, n};
}

inline auto operator "" _mkcol(const char* s, std::size_t n)
{
    return wrap_mkcol{s, n};
}

inline auto operator "" _move(const char* s, std::size_t n)
{
    return wrap_move{s, n};
}

inline auto operator "" _propfind(const char* s, std::size_t n)
{
    return wrap_propfind{s, n};
}

inline auto operator "" _proppatch(const char* s, std::size_t n)
{
    return wrap_proppatch{s, n};
}

inline auto operator "" _search(const char* s, std::size_t n)
{
    return wrap_search{s, n};
}

inline auto operator "" _unlock(const char* s, std::size_t n)
{
    return wrap_unlock{s, n};
}

inline auto operator "" _bind(const char* s, std::size_t n)
{
    return wrap_bind{s, n};
}

inline auto operator "" _rebind(const char* s, std::size_t n)
{
    return wrap_rebind{s, n};
}

inline auto operator "" _unbind(const char* s, std::size_t n)
{
    return wrap_unbind{s, n};
}

inline auto operator "" _acl(const char* s, std::size_t n)
{
    return wrap_acl{s, n};
}

inline auto operator "" _report(const char* s, std::size_t n)
{
    return wrap_report{s, n};
}

inline auto operator "" _mkactivity(const char* s, std::size_t n)
{
    return wrap_mkactivity{s, n};
}

inline auto operator "" _checkout(const char* s, std::size_t n)
{
    return wrap_checkout{s, n};
}

inline auto operator "" _merge(const char* s, std::size_t n)
{
    return wrap_merge{s, n};
}

inline auto operator "" _msearch(const char* s, std::size_t n)
{
    return wrap_msearch{s, n};
}

inline auto operator "" _notify(const char* s, std::size_t n)
{
    return wrap_notify{s, n};
}

inline auto operator "" _subscribe(const char* s, std::size_t n)
{
    return wrap_subscribe{s, n};
}

inline auto operator "" _unsubscribe(const char* s, std::size_t n)
{
    return wrap_unsubscribe{s, n};
}

inline auto operator "" _patch(const char* s, std::size_t n)
{
    return wrap_patch{s, n};
}

inline auto operator "" _purge(const char* s, std::size_t n)
{
    return wrap_purge{s, n};
}

inline auto operator "" _mkcalendar(const char* s, std::size_t n)
{
    return wrap_mkcalendar{s, n};
}

inline auto operator "" _link(const char* s, std::size_t n)
{
    return wrap_link{s, n};
}

inline auto operator "" _unlink(const char* s, std::size_t n)
{
    return wrap_unlink{s, n};
}

inline auto operator "" _all(const char* s, std::size_t n)
{
    return wrap_all{s, n};
}

} // namespace literal

#endif // BEAST_HTTP_SERVER_HPP

#ifdef BEAST_HTTP_CLIENT_HPP

/// \brief Class for communication with a remote host (SSL)
/// \tparam Body type for response message
template<class ResBody>
class client_impl : private http::client_impl<ResBody>{

    template<class Callback0>
    bool process(std::string const & host, uint32_t port, Callback0 && on_error_handler){
        connection_p_ = http::base::processor::get()
                .create_connection<base::connection>(ctx_, host,
                                                     port,
                                                     [this, on_error = std::forward<Callback0>(on_error_handler)](const boost::system::error_code & ec){
            if(ec){
                http::base::fail(ec, "connect");
                on_error(ec);
                return;
            }

            session<false, ResBody>::on_connect(connection_p_, on_connect, on_handshake, on_message);
        });

        if(!connection_p_)
            return false;

        return true;
    }

    base::connection::ptr connection_p_;
    boost::asio::ssl::context & ctx_;

public:

    std::function<void (session<false, ResBody>&)> on_connect;
    std::function<void (session<false, ResBody>&)> on_handshake;
    std::function<void (boost::beast::http::response<ResBody>&, session<false, ResBody>&)> on_message;

    explicit client_impl(boost::asio::ssl::context & ctx)
        :ctx_{ctx},
        connection_p_{nullptr}
    {}

    template<class Callback0>
    bool invoke(std::string const & host, uint32_t port, Callback0 && on_error_handler){
        return process(host, port, std::forward<Callback0>(on_error_handler));
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

#endif // BEAST_HTTP_CLIENT_HPP

} // namespace ssl

} // namespace http


#endif // BEAST_HTTP_SSL_HPP
