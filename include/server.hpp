#ifndef BEAST_HTTP_SERVER_HPP
#define BEAST_HTTP_SERVER_HPP

#include "base.hpp"
#include "session.hpp"
#include "list_cb.hpp"
#include "router.hpp"


namespace http {

/// \brief HTTP server class
/// \tparam Body type for request message
template<class ReqBody>
class server_impl{

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
        base::processor::get().add_listener(address, port,
                                            std::bind(&session<true, ReqBody>::on_accept,
                                                        std::placeholders::_1,
                                                        std::cref(resource_map_cb_p_),
                                                        std::cref(method_map_cb_p_),
                                                        std::cref(on_accept)
                                                        )
                                            )->run();
    }

public:

    std::function<void(session<true, ReqBody>&)> on_accept;

    explicit server_impl() :
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

}; // server_impl class

// default server implementation used std::string as body message
using server = server_impl<boost::beast::http::string_body>;

// the list wrapped methods (string literal only)
struct wrap_get{ // GET

    const char * s_;
    std::size_t n_;

    wrap_get(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.get({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_post{ // POST

    const char * s_;
    std::size_t n_;

    wrap_post(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.post({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_put{ // PUT

    const char * s_;
    std::size_t n_;

    wrap_put(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.put({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_head{ // HEAD

    const char * s_;
    std::size_t n_;

    wrap_head(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.head({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_delete_{ // DELETE

    const char * s_;
    std::size_t n_;

    wrap_delete_(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.delete_({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_options{ // OPTIONS

    const char * s_;
    std::size_t n_;

    wrap_options(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.options({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_connect{ // CONNECT

    const char * s_;
    std::size_t n_;

    wrap_connect(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.connect({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_trace{ // TRACE

    const char * s_;
    std::size_t n_;

    wrap_trace(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.trace({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_copy{ // COPY

    const char * s_;
    std::size_t n_;

    wrap_copy(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.copy({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_lock{ // LOCK

    const char * s_;
    std::size_t n_;

    wrap_lock(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.lock({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_mkcol{ // MKCOL

    const char * s_;
    std::size_t n_;

    wrap_mkcol(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.mkcol({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_move{ // MOVE

    const char * s_;
    std::size_t n_;

    wrap_move(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.move({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_propfind{ // PROPFIND

    const char * s_;
    std::size_t n_;

    wrap_propfind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.propfind({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_proppatch{ // PROPPATCH

    const char * s_;
    std::size_t n_;

    wrap_proppatch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.proppatch({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_search{ // SEARCH

    const char * s_;
    std::size_t n_;

    wrap_search(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.search({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_unlock{ // UNLOCK

    const char * s_;
    std::size_t n_;

    wrap_unlock(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.unlock({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_bind{ // BIND

    const char * s_;
    std::size_t n_;

    wrap_bind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.bind({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_rebind{ // REBIND

    const char * s_;
    std::size_t n_;

    wrap_rebind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.rebind({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_unbind{ // UNBIND

    const char * s_;
    std::size_t n_;

    wrap_unbind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.unbind({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_acl{ // ACL

    const char * s_;
    std::size_t n_;

    wrap_acl(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.acl({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_report{ // REPORT

    const char * s_;
    std::size_t n_;

    wrap_report(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.report({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_mkactivity{ // MKACTIVITY

    const char * s_;
    std::size_t n_;

    wrap_mkactivity(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.mkactivity({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_checkout{ // CHECKOUT

    const char * s_;
    std::size_t n_;

    wrap_checkout(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.checkout({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_merge{ // MERGE

    const char * s_;
    std::size_t n_;

    wrap_merge(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.merge({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_msearch{ // MSEARCH

    const char * s_;
    std::size_t n_;

    wrap_msearch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.msearch({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_notify{ // NOTIFY

    const char * s_;
    std::size_t n_;

    wrap_notify(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.notify({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_subscribe{ // SUBSCRIBE

    const char * s_;
    std::size_t n_;

    wrap_subscribe(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.subscribe({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_unsubscribe{ // UNSUBSCRIBE

    const char * s_;
    std::size_t n_;

    wrap_unsubscribe(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.options({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_patch{ // PATCH

    const char * s_;
    std::size_t n_;

    wrap_patch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.patch({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_purge{ // PURGE

    const char * s_;
    std::size_t n_;

    wrap_purge(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.purge({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_mkcalendar{ // MKCALENDAR

    const char * s_;
    std::size_t n_;

    wrap_mkcalendar(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.mkcalendar({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_link{ // LINK

    const char * s_;
    std::size_t n_;

    wrap_link(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.link({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_unlink{ // UNLINK

    const char * s_;
    std::size_t n_;

    wrap_unlink(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.unlink({s_, n_}, std::forward<Callback>(f)...);
    }

};

struct wrap_all{ // ALL

    const char * s_;
    std::size_t n_;

    wrap_all(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class ReqBody, class... Callback>
    void bind(server_impl<ReqBody> & instance, Callback&&... f){
        instance.all({s_, n_}, std::forward<Callback>(f)...);
    }

};

} // namespace http

/// \param s - pointer to resource regex
/// \param n - length of string

inline auto operator "" _get(const char* s, std::size_t n)
{
    return http::wrap_get{s, n};
}

inline auto operator "" _post(const char* s, std::size_t n)
{
    return http::wrap_post{s, n};
}

inline auto operator "" _put(const char* s, std::size_t n)
{
    return http::wrap_put{s, n};
}

inline auto operator "" _head(const char* s, std::size_t n)
{
    return http::wrap_put{s, n};
}

inline auto operator "" _delete_(const char* s, std::size_t n)
{
    return http::wrap_delete_{s, n};
}

inline auto operator "" _options(const char* s, std::size_t n)
{
    return http::wrap_options{s, n};
}

inline auto operator "" _connect(const char* s, std::size_t n)
{
    return http::wrap_connect{s, n};
}

inline auto operator "" _trace(const char* s, std::size_t n)
{
    return http::wrap_trace{s, n};
}

inline auto operator "" _copy(const char* s, std::size_t n)
{
    return http::wrap_copy{s, n};
}

inline auto operator "" _lock(const char* s, std::size_t n)
{
    return http::wrap_lock{s, n};
}

inline auto operator "" _mkcol(const char* s, std::size_t n)
{
    return http::wrap_mkcol{s, n};
}

inline auto operator "" _move(const char* s, std::size_t n)
{
    return http::wrap_move{s, n};
}

inline auto operator "" _propfind(const char* s, std::size_t n)
{
    return http::wrap_propfind{s, n};
}

inline auto operator "" _proppatch(const char* s, std::size_t n)
{
    return http::wrap_proppatch{s, n};
}

inline auto operator "" _search(const char* s, std::size_t n)
{
    return http::wrap_search{s, n};
}

inline auto operator "" _unlock(const char* s, std::size_t n)
{
    return http::wrap_unlock{s, n};
}

inline auto operator "" _bind(const char* s, std::size_t n)
{
    return http::wrap_bind{s, n};
}

inline auto operator "" _rebind(const char* s, std::size_t n)
{
    return http::wrap_rebind{s, n};
}

inline auto operator "" _unbind(const char* s, std::size_t n)
{
    return http::wrap_unbind{s, n};
}

inline auto operator "" _acl(const char* s, std::size_t n)
{
    return http::wrap_acl{s, n};
}

inline auto operator "" _report(const char* s, std::size_t n)
{
    return http::wrap_report{s, n};
}

inline auto operator "" _mkactivity(const char* s, std::size_t n)
{
    return http::wrap_mkactivity{s, n};
}

inline auto operator "" _checkout(const char* s, std::size_t n)
{
    return http::wrap_checkout{s, n};
}

inline auto operator "" _merge(const char* s, std::size_t n)
{
    return http::wrap_merge{s, n};
}

inline auto operator "" _msearch(const char* s, std::size_t n)
{
    return http::wrap_msearch{s, n};
}

inline auto operator "" _notify(const char* s, std::size_t n)
{
    return http::wrap_notify{s, n};
}

inline auto operator "" _subscribe(const char* s, std::size_t n)
{
    return http::wrap_subscribe{s, n};
}

inline auto operator "" _unsubscribe(const char* s, std::size_t n)
{
    return http::wrap_unsubscribe{s, n};
}

inline auto operator "" _patch(const char* s, std::size_t n)
{
    return http::wrap_patch{s, n};
}

inline auto operator "" _purge(const char* s, std::size_t n)
{
    return http::wrap_purge{s, n};
}

inline auto operator "" _mkcalendar(const char* s, std::size_t n)
{
    return http::wrap_mkcalendar{s, n};
}

inline auto operator "" _link(const char* s, std::size_t n)
{
    return http::wrap_link{s, n};
}

inline auto operator "" _unlink(const char* s, std::size_t n)
{
    return http::wrap_unlink{s, n};
}

inline auto operator "" _all(const char* s, std::size_t n)
{
    return http::wrap_all{s, n};
}

#endif // BEAST_HTTP_SERVER_HPP
