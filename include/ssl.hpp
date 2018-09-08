#ifndef BEAST_HTTP_SSL_HPP
#define BEAST_HTTP_SSL_HPP

#include <boost/asio/ssl/stream.hpp>
#include <base.hpp>
#include <session.hpp>
#include <list_cb.hpp>
#include <router.hpp>

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

    template <class F, class B>
    void async_handshake_server(B& buf, F&& f){
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

    template<class F>
    void async_shutdown(F&& f){
        stream_.async_shutdown(
                    boost::asio::bind_executor(
                        strand_, std::forward<F>(f)));
    }

    auto & stream(){
        return stream_;
    }

};

} // namespace base

/// \brief session class. Handles an HTTPS server connection
/// \tparam Type of body request message
template<bool isServer, class Body>
class session  : private cb_invoker, private boost::noncopyable,
        public std::enable_shared_from_this<session<true, Body> >
{

    using list_cb_t = list_cb<boost::beast::http::request<Body>, session<true, Body>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    bool handshake = false;

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

    explicit session(boost::asio::ssl::context& ctx,
                     boost::asio::ip::tcp::socket&& socket,
                     boost::beast::flat_buffer&& buffer,
                     const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                     const std::shared_ptr<method_map_t> & method_map_cb_p)
        : timer_p_{std::make_shared<http::base::timer>(socket.get_executor(),
                                                       (std::chrono::steady_clock::time_point::max)())},
          connection_p_{std::make_shared<base::connection>(ctx, std::move(socket))},
          buffer_{std::move(buffer)},
          resource_map_cb_p_{resource_map_cb_p},
          method_map_cb_p_{method_map_cb_p},
          queue_{*this}
    {}

    static void on_accept(boost::asio::ssl::context& ctx,
                          boost::asio::ip::tcp::socket&& socket,
                          boost::beast::flat_buffer&& buffer,
                          const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                          const std::shared_ptr<method_map_t> & method_map_cb_p,
                          const std::function<void(session<true, ReqBody>&)> & on_accept_cb)
    {
        auto new_session_p = std::make_shared<session<true, Body> >(ctx, std::move(socket), std::move(buffer), resource_map_cb_p, method_map_cb_p);
        if(on_accept_cb)
            on_accept_cb(*new_session_p);
    }

    void do_handshake(){
        if(handshake)
            return;

        connection_p_->async_handshake_server(buffer_,
                                       std::bind(&session<true, Body>::on_handshake,
                                                 this->shared_from_this(),
                                                 std::placeholders::_1,
                                                 std::placeholders::_2));
    }

    void do_read(){
        if(!handshake)
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
        if(!handshake)
            return;

        queue_(std::forward<Responce>(msg));
    }

    void do_close(){
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

protected:

    void on_handshake(const boost::system::error_code & ec, std::size_t bytes_used)
    {
        if(ec)
            return http::base::fail(ec, "handshake");

        handshake = true;

        // Consume the portion of the buffer used by the handshake
        buffer_.consume(bytes_used);

        do_read();
    }

    void on_shutdown(const boost::system::error_code & ec)
    {
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
    void do_write(Request && msg){
        if(!handshake)
            return;

        auto sp = std::make_shared<std::decay_t<Request>>(std::forward<Request>(msg));
        msg_p_ = sp;

        connection_p_->async_write(*sp,
                                 std::bind(&session<false, Body>::on_write, this->shared_from_this(),
                                             std::placeholders::_1,
                                             std::placeholders::_2
                                             ));
    }

    void do_close(){
        boost::system::error_code ec;
        // Gracefully close the socket
        connection_p_->async_shutdown(std::bind(
                                          &session::on_shutdown,
                                          this->shared_from_this(),
                                          std::placeholders::_1));

    }

private:

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

    void on_write(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return http::base::fail(ec, "write");

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
                                                      std::placeholders::_2,
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

#endif // BEAST_HTTP_SERVER_HPP

#ifdef BEAST_HTTP_CLIENT_HPP

/// \brief Class for communication with a remote host (SSL)
/// \tparam Body type for response message
template<class ResBody>
class client_impl : private http::client_impl<ResBody>{

    void process(std::string const & host, uint32_t port){
        connection_p_ = http::base::processor::get()
                .create_connection<base::connection>(ctx_, host,
                                                     port,
                                                     [this](const boost::system::error_code & ec){
            if(ec){
                connection_p_->stream().get_executor().context().stop();
                return http::base::fail(ec, "connect");
            }

            session<false, ResBody>::on_connect(std::ref(connection_p_), std::cref(on_connect), std::cref(on_handshake), std::cref(on_message));
        });

        if(!connection_p_){
            on_connect = {};
            on_handshake = {};
            on_message = {};
            http::base::processor::get().stop();
        }
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

    void invoke(std::string const & host, uint32_t port){
        process(host, port);
    }
    /// \brief Get request a resource
    /// \param Remote host (www.example.com, 127.0.0.1 and etc.)
    /// \param Session port (80, 8080)
    /// \param handler informing you of a successful connection
    /// Callback1 signature : template<class Session>
    ///                      void (Session & session)
    /// \param handler informing you of a handshake
    /// Callback2 signature : template<class Session>
    ///                      void (Session & session)
    /// \param handler on the received message
    /// Callback3 signature : template<class Message>
    ///                      void (Message & message, Session & session)
    template<class Callback1, class Callback2, class Callback3>
    void invoke(std::string const & host, uint32_t port, Callback1 && on_connect_handler, Callback2 && on_handshake_handler, Callback3 && on_message_handler){

        on_connect = std::forward<Callback1>(on_connect_handler);
        on_handshake = std::forward<Callback2>(on_handshake_handler);
        on_message = std::forward<Callback3>(on_message_handler);

        process(host, port);
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

#endif // BEAST_HTTP_CLIENT_HPP

} // namespace ssl

} // namespace http


#endif // BEAST_HTTP_SSL_HPP
