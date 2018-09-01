#ifndef SSL_HPP
#define SSL_HPP

#include <boost/asio/ssl/stream.hpp>
#include <server.hpp>

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

public:

    using ReqBody = Body;

    explicit session(boost::asio::ssl::context& ctx,
                     boost::asio::ip::tcp::socket&& socket,
                     boost::beast::flat_buffer&& buffer,
                     const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                     const std::shared_ptr<method_map_t> & method_map_cb_p)
        : connection_p_{std::make_shared<base::connection>(ctx, std::move(socket))},
          buffer_{std::move(buffer)},
          resource_map_cb_p_{resource_map_cb_p},
          method_map_cb_p_{method_map_cb_p}
    {}

    template<class Callback>
    static void on_accept(boost::asio::ssl::context& ctx,
                          boost::asio::ip::tcp::socket&& socket,
                          boost::beast::flat_buffer&& buffer,
                          const std::shared_ptr<resource_map_t> & resource_map_cb_p,
                          const std::shared_ptr<method_map_t> & method_map_cb_p,
                          const Callback & handler)
    {
        auto new_session_p = std::make_shared<session<true, Body> >(ctx, std::move(socket), std::move(buffer), resource_map_cb_p, method_map_cb_p);
        handler(*new_session_p);
    }

    void do_handshake(){
        if(handshake)
            return;

        connection_p_->async_handshake_server(buffer_,
                                       std::bind(&session<true, Body>::on_handshake,
                                                 this->shared_from_this(),
                                                 std::placeholders::_1));
    }

    void do_read(){
        if(!handshake)
            return;

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

        auto sp = std::make_shared<std::decay_t<Responce>>(std::forward<Responce>(msg));
        msg_p_ = sp;

        connection_p_->async_write(*sp,
                                 std::bind(&session<true, Body>::on_write, this->shared_from_this(),
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             sp->need_eof()));
    }

    void do_close(){
        connection_p_->async_shutdown(std::bind(
                                          &session<true, Body>::on_shutdown,
                                          this->shared_from_this(),
                                          std::placeholders::_1));
    }

protected:

    void on_handshake(const boost::system::error_code & ec)
    {
        if(ec)
            return http::base::fail(ec, "handshake");

        handshake = true;

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

        // We're done with the response so delete it
        msg_p_.reset();

        // Read another request
        do_read();
    }

    void process_request()
    {

        resource_t target = req_.target();
        method_t method = req_.method();

        if(method_map_cb_p_){
            auto method_pos = method_map_cb_p_->find(method);
            if(method_pos != method_map_cb_p_->end()){

                auto & resource_map = method_pos->second;

                for(const auto & value : resource_map){
                    const boost::regex e(value.first, boost::regex::perl | boost::regex::no_except);
                    if(boost::regex_match(std::string(target.data(), target.size()), e)){
                        auto const & cb_p = value.second;

                        if(cb_p)
                            return invoke_cb(req_, *this, *cb_p);

                    }
                }
            }
        }

        if(resource_map_cb_p_)
            for(const auto & value : *resource_map_cb_p_){
                const boost::regex e(value.first, boost::regex::perl | boost::regex::no_except);
                if(boost::regex_match(std::string(target.data(), target.size()), e)){
                    auto const & cb_p = value.second;

                    if(cb_p)
                        return invoke_cb(req_, *this, *cb_p);

                }
            }

        return do_read();
    }

    base::connection::ptr connection_p_;
    boost::beast::flat_buffer buffer_;
    std::shared_ptr<resource_map_t> resource_map_cb_p_;
    std::shared_ptr<method_map_t> method_map_cb_p_;
    boost::beast::http::request<Body> req_;
    std::shared_ptr<void> msg_p_;

};

/// \brief session class. Handles an HTTPS client connection
/// \tparam Type of body response message
template<class Body>
class session<false, Body>  : private cb_invoker, private boost::noncopyable,
        public std::enable_shared_from_this<session<false, Body> >{

public:

    using list_cb_t = list_cb<boost::beast::http::response<Body>, session<false, Body> >;

    explicit session(const base::connection::ptr & connection_p,
                     const typename list_cb_t::ptr & response_cb_p,
                     const std::function<void (session<false, Body> &)> & handshake_cb)
        : connection_p_{connection_p},
          response_cb_p_{response_cb_p},
          handshake_cb_{handshake_cb}
    {}

    template<class Callback>
    static void on_connect(const base::connection::ptr & connection_p,
                           const typename list_cb_t::ptr & response_cb_p,
                           const std::function<void (session<false, Body> &)> & handshake_cb,
                           const Callback & handler){
        auto new_session_p = std::make_shared<session<false, Body>>(connection_p, response_cb_p, handshake_cb);
        handler(*new_session_p);
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
        handshake_cb_(*this);
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

        if(response_cb_p_)
            invoke_cb(res_, *this, *response_cb_p_);

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
    base::connection::ptr connection_p_;
    typename list_cb_t::ptr response_cb_p_;
    std::function<void (session<false, Body> &)> handshake_cb_;
    boost::beast::http::response<Body> res_;
    std::shared_ptr<void> msg_p_;
    boost::beast::flat_buffer buffer_;

}; // class session

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

    bool check_status(){
        if(status_){
            http::base::fail("Adding a handler is not available when the server is running");
            return status_;
        }
        return status_;
    }

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

protected:

    bool status_;
    boost::asio::ssl::context & ctx_;

public:

    // new ssl context
    explicit server_impl(boost::asio::ssl::context & ctx) :
        ctx_{ctx},
        status_{false},
        basic_router_{nullptr},
        chain_router_{nullptr},
        resource_map_cb_p_{nullptr},
        method_map_cb_p_{nullptr}
    {}

    typename chain_r::ref route(const resource_regex_t & path_to_resource){
        if(!check_status())
            get_chain_router()->save_to_res_regex(path_to_resource);

        return get_chain_router().operator *();
    }
    /// Callback signature : template<class Message, class Session /*, class Next (optional)*/>
    ///                     void (Message & message, Session & session /*, Next & next (optional)*/)
    /// \brief Adds a handler for GET method
    template<class... Callback>
    void get(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->get(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for POST method
    template<class... Callback>
    void post(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->post(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PUT method
    template<class... Callback>
    void put(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->put(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for HEAD method
    template<class... Callback>
    void head(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->head(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for DELETE method
    template<class... Callback>
    void delete_(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->delete_(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for OPTIONS method
    template<class... Callback>
    void options(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->options(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for CONNECT method
    template<class... Callback>
    void connect(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->connect(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for TRACE method
    template<class... Callback>
    void trace(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->trace(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for COPY method
    template<class... Callback>
    void copy(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->copy(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for LOCK method
    template<class... Callback>
    void lock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->lock(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MKCOL method
    template<class... Callback>
    void mkcol(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->mkcol(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MOVE method
    template<class... Callback>
    void move(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->move(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PROPFIND method
    template<class... Callback>
    void propfind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->propfind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PROPPATCH method
    template<class... Callback>
    void proppatch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->proppatch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for SEARCH method
    template<class... Callback>
    void search(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->search(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNLOCK method
    template<class... Callback>
    void unlock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->unlock(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for BIND method
    template<class... Callback>
    void bind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->bind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for REBIND method
    template<class... Callback>
    void rebind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->rebind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNBIND method
    template<class... Callback>
    void unbind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->unbind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for ACL method
    template<class... Callback>
    void acl(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->acl(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // subversion
    /// \brief Adds a handler for REPORT method
    template<class... Callback>
    void report(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->report(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MKACTIVITY method
    template<class... Callback>
    void mkactivity(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->mkactivity(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for CHECKOUT method
    template<class... Callback>
    void checkout(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->checkout(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MERGE method
    template<class... Callback>
    void merge(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->merge(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // upnp
    /// \brief Adds a handler for MSEARCH method
    template<class... Callback>
    void msearch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->msearch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for NOTIFY method
    template<class... Callback>
    void notify(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->notify(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for SUBSCRIBE method
    template<class... Callback>
    void subscribe(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->subscribe(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNSUBSCRIBE method
    template<class... Callback>
    void unsubscribe(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->unsubscribe(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // RFC-5789
    /// \brief Adds a handler for PATCH method
    template<class... Callback>
    void patch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->patch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PURGE method
    template<class... Callback>
    void purge(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->purge(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // CalDAV
    /// \brief Adds a handler for MKCALENDAR method
    template<class... Callback>
    void mkcalendar(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->mkcalendar(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    // RFC-2068, section 19.6.1.2
    /// \brief Adds a handler for LINK method
    template<class... Callback>
    void link(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->link(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNLINK method
    template<class... Callback>
    void unlink(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->unlink(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for the requested resource by default
    /// \note If the handler for the requested resource with method is not found, this on is called
    template<class... Callback>
    void all(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) & {
        if(!check_status())
            get_basic_router()->all(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }

    template<class... Types>
    auto param(){
        if(check_status())
            throw std::runtime_error("The param method is not available in current time");

        return get_basic_router()->template param<Types...>();
    }

    /// \brief Start accepting incoming connections
    /// \param Listening interface
    /// \param port
    /// \param Handler on new incoming connection
    /// Callback signature : template<class Session>
    ///                     void (Session & session)
    template<class Callback>
    void listen(const std::string & address, uint32_t port, Callback && on_accept_handler){
        if(status_)
            return http::base::fail("server is already running");

        http::base::processor::get().add_listener(address, port,
                                            std::bind(&session<true, ReqBody>::template on_accept<Callback>,
                                                      std::ref(ctx_),
                                                      std::placeholders::_1,
                                                      std::placeholders::_2,
                                                      resource_map_cb_p_,
                                                      method_map_cb_p_,
                                                      std::forward<Callback>(on_accept_handler)
                                                      )
                                            )->run();
        status_ = true;
    }


    basic_r BasicRouter() noexcept{
        return {};
    }

    chain_r ChainRouter() noexcept{
        return {};
    }

    void use(const resource_regex_t & path_to_resource, typename basic_r::cref other){
        if(!check_status())
            get_basic_router()->use(path_to_resource, other);
    }

    void use(const resource_regex_t & path_to_resource, typename chain_r::cref other){
        if(!check_status())
            get_chain_router()->use(path_to_resource, other);
    }

    void use(typename basic_r::cref other){
        if(!check_status())
            get_basic_router()->use("", other);
    }

    void use(typename chain_r::cref other){
        if(!check_status())
            get_chain_router()->use("", other);
    }

};

using server = server_impl<boost::beast::http::string_body>;

/// \brief Class for communication with a remote host (SSL)
/// \tparam Body type for response message
template<class ResBody>
class client_impl{

public:

    using list_cb_t = list_cb<boost::beast::http::response<ResBody>, session<false, ResBody>>;

    explicit client_impl(boost::asio::ssl::context & ctx) : status{false},
        ctx_{ctx},
        connection_p_{nullptr},
        response_cb_p_{nullptr}
    {}

    /// \brief Get request a resource
    /// \param Remote host (www.example.com, 127.0.0.1 and etc.)
    /// \param Session port (80, 8080)
    /// \param handler informing you of a successful connection
    /// Callback1 signature : template<class Session>
    ///                      void (Session & session)
    /// \param handler on the received message
    /// Callback2 signature : template<class Message>
    ///                      void (Message & message, Session & session)
    template<class Callback1, class Callback2, class Callback3>
    void invoke(std::string const & host, uint32_t port,
                Callback1 && on_connect_handler, Callback2 && on_handshake_handler, Callback3 && on_receive_handler){
        handshake_cb_ = std::forward<Callback2>(on_handshake_handler);

        typename list_cb_t::L cb{
            std::bind<void>(
                        std::forward<Callback3>(on_receive_handler),
                        std::placeholders::_1,
                        std::placeholders::_2
                        )
        };

        response_cb_p_ = std::make_shared<list_cb_t>(cb);

        connection_p_ = http::base::processor::get()
                .create_connection<base::connection>(ctx_, host,
                                                     port,
                                                     [this, handler_ = std::forward<Callback1>(on_connect_handler)](const boost::system::error_code & ec){
            if(ec){
                connection_p_->stream().get_executor().context().stop();
                return http::base::fail(ec, "connect");
            }

            session<false, ResBody>::on_connect(connection_p_, response_cb_p_, handshake_cb_, handler_);
        });

        if(!connection_p_){
            response_cb_p_ = {};
            http::base::processor::get().stop();
        }
    }

private:

    bool status;
    boost::asio::ssl::context & ctx_;
    base::connection::ptr connection_p_;
    typename list_cb_t::ptr response_cb_p_;
    std::function<void (session<false, ResBody> &)> handshake_cb_;

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace ssl

} // namespace http


#endif // SSL_HPP
