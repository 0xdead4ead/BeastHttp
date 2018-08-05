#ifndef SERVER_HPP
#define SERVER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <session.hpp>
#include <list_cb.hpp>
#include <router.hpp>


namespace http {

/// \brief HTTP server class
/// \tparam Body type for request message
template<class ReqBody>
class server_impl{

    using basic_r = basic_router<ReqBody>;
    using chain_r = chain_router<ReqBody>;
    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

    bool status_;
    std::shared_ptr<basic_r> basic_router_;
    std::shared_ptr<chain_r> chain_router_;
    std::shared_ptr<resource_map_t> resource_map_cb_p_;
    std::shared_ptr<method_map_t> method_map_cb_p_;

    bool check_status(){
        if(status_){
            base::fail("Adding a handler is not available when the server is running");
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

public:

    explicit server_impl() :
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
    ///                     void (const Message & message, Session & session /*, Next & next (optional)*/)
    /// \brief Adds a handler for GET method
    template<class... Callback>
    void get(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->get(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for POST method
    template<class... Callback>
    void post(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->post(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PUT method
    template<class... Callback>
    void put(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->put(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for HEAD method
    template<class... Callback>
    void head(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->head(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for DELETE method
    template<class... Callback>
    void delete_(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->delete_(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for OPTIONS method
    template<class... Callback>
    void options(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->options(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for CONNECT method
    template<class... Callback>
    void connect(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->connect(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for TRACE method
    template<class... Callback>
    void trace(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->trace(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for COPY method
    template<class... Callback>
    void copy(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->copy(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for LOCK method
    template<class... Callback>
    void lock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->lock(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MKCOL method
    template<class... Callback>
    void mkcol(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->mkcol(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for MOVE method
    template<class... Callback>
    void move(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->move(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PROPFIND method
    template<class... Callback>
    void propfind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->propfind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for PROPPATCH method
    template<class... Callback>
    void proppatch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->proppatch(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for SEARCH method
    template<class... Callback>
    void search(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->search(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNLOCK method
    template<class... Callback>
    void unlock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->unlock(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for BIND method
    template<class... Callback>
    void bind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->bind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for REBIND method
    template<class... Callback>
    void rebind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->rebind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for UNBIND method
    template<class... Callback>
    void unbind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->unbind(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for ACL method
    template<class... Callback>
    void acl(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->acl(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }
    /// \brief Adds a handler for the requested resource by default
    /// \note If the handler for the requested resource with method is not found, this on is called
    template<class... Callback>
    void all(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        if(!check_status())
            get_basic_router()->all(path_to_resource, std::forward<Callback>(on_resource_handlers)...);
    }

    /// \brief Start accepting incoming connections
    /// \param Listening interface
    /// \param port
    /// \param Handler on new incoming connection
    /// Callback signature : template<class Session>
    ///                     void (const Session & session)
    template<class Callback>
    void listen(const std::string & address, uint32_t port, Callback && on_accept_handler){
        if(status_)
            return base::fail("server is already running");

        base::processor::get().add_listener(address, port,
                                            boost::bind(&session<true, ReqBody>::template on_accept<Callback>,
                                                        boost::placeholders::_1,
                                                        resource_map_cb_p_,
                                                        method_map_cb_p_,
                                                        std::forward<Callback>(on_accept_handler)
                                                        )
                                            )->accept_start();
        status_ = true;
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

}; // server_impl class

using server = server_impl<boost::beast::http::string_body>;

} // namespace http

#endif // SERVER_HPP
