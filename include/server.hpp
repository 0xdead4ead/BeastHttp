#ifndef BEAST_HTTP_SERVER_HPP
#define BEAST_HTTP_SERVER_HPP

#include "base.hpp"
#include "session.hpp"
#include "router.hpp"


namespace http {

/// \brief HTTP server class
/// \tparam Body type for request message
template<class ReqBody>
class server_impl{

    using self_type = server_impl<ReqBody>;
    using session_type = session<true, ReqBody>;
    using basic_r = basic_router<session_type>;
    using chain_r = chain_router<session_type>;
    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session_type>;
    using resource_map_t = boost::unordered_map<resource_regex_t, typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    using on_accept_fn
            = std::function<void (session_type&)>;

    using on_error_fn
            = std::function<void (boost::beast::error_code const &)>;

    bool is_started = false;

    std::shared_ptr<basic_r> basic_router_;
    std::shared_ptr<chain_r> chain_router_;

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

    void on_accept_(const boost::system::error_code& ec){
        if(ec){
            base::fail(ec, "accept");
            if(on_error)
                on_error(ec);
        }
        else
            session<true, ReqBody>::on_accept
                    (listener_p_->get_socket(), resource_map_cb_p_, method_map_cb_p_, on_accept, on_error);

        // Accept another connection
        listener_p_->do_accept(std::bind(&self_type::on_accept_, this, std::placeholders::_1));
    }

    bool process(const std::string & address, uint32_t port){
        auto ec = boost::system::error_code{};
        auto resolved = processor_.resolve(address, port, ec);

        if(ec){
            if(on_error)
                on_error(ec);

            return false;
        }

        auto listener = base::listener{processor_.io_service()};

        if(auto ec = listener.init(resolved)){
            if(on_error)
                on_error(ec);

            return false;
        }

        listener_p_ = processor_.add_listener(std::move(listener)).shared_from_this();
        listener_p_->run(std::bind(&self_type::on_accept_, this, std::placeholders::_1));

        is_started = true;
        return true;
    }

protected:

    base::listener::ptr listener_p_;
    base::processor& processor_;

    std::shared_ptr<resource_map_t> resource_map_cb_p_;
    std::shared_ptr<method_map_t> method_map_cb_p_;

public:

    on_accept_fn on_accept;
    on_error_fn on_error;

    explicit server_impl()
        : processor_{base::processor::get()},
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
    void get(const resource_regex_t & path_to_resource, Callback && ... on_message) & {
        get_basic_router()->get(path_to_resource, std::forward<Callback>(on_message)...);
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

    bool listen(const std::string & address, uint32_t port){
        if(is_started)
            return true;

        return process(address, port);
    }

    template<class Callback1, class Callback2>
    bool listen(const std::string & address, uint32_t port,
                Callback1 && on_accept_handler, Callback2 && on_error_handler){
        if(is_started)
            return true;

        on_accept = std::forward<Callback1>(on_accept_handler);
        on_error = std::forward<Callback2>(on_error_handler);
        return process(address, port);
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

}; // server_impl class

// default server implementation used std::string as body message
using server = server_impl<boost::beast::http::string_body>;

} // namespace http

#endif // BEAST_HTTP_SERVER_HPP
