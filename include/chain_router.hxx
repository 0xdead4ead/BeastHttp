#if not defined BEASTHTTP_CHAIN_ROUTER_HXX
#define BEASTHTTP_CHAIN_ROUTER_HXX

#include <base/router.hxx>

namespace _0xdead4ead {
namespace http {

template<class Session>
class chain_router : public base::router<Session>
{
    using base_type = base::router<Session>;

    using self_type = chain_router;

    class chain_node;
    friend class chain_node;

public:

    using session_type = typename base_type::session_type;

    using storage_type = typename base_type::storage_type;

    using resource_map_type = typename base_type::resource_map_type;

    using method_map_type = typename base_type::method_map_type;

    using resource_regex_type = typename base_type::resource_regex_type;

    using method_type = typename base_type::method_type;

    using regex_type = typename base_type::regex_type;

    using request_type = typename base_type::request_type;

    using chain_node_type = chain_node;

    chain_router() noexcept
        : base_type{resource_map_, method_map_}
    {}

    chain_node
    route(resource_regex_type const& path_to_resource) &
    {
        save_to_res_regex(path_to_resource);
        return chain_node{*this};
    }

    void
    use(resource_regex_type const& path_to_resource, base_type const& other)
    {
        base_type::use(path_to_resource, other);
    }

    void
    use(base_type const& other)
    {
        base_type::use("", other);
    }

    template<class Pack>
    auto
    param(typename regex_type::flag_type flags)
    -> decltype (std::declval<base_type>().template param<self_type, Pack>(*this, flags))
    {
        return base_type::template param<self_type, Pack>(*this, flags);
    }

private:

    class chain_node
    {
        using router_type = self_type;

        using node_type = chain_node;

    public:

        explicit
        chain_node(router_type& router) noexcept
            : router_{router}
        {}

        template<class... OnRequest>
        auto
        get(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::get, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        post(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::post, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        put(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::put, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        head(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::head, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        delete_(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::delete_, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        options(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::options, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        connect(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::connect, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        trace(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::trace, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        //Webdav

        template<class... OnRequest>
        auto
        copy(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::copy, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        lock(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::lock, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        mkcol(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::mkcol, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        move(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::move, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        propfind(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::propfind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        proppatch(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::proppatch, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        search(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::search, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        unlock(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unlock, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        bind(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::bind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        rebind(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::rebind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        unbind(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unbind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        acl(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::acl, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // subversion

        template<class... OnRequest>
        auto
        report(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::report, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        mkactivity(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::mkactivity, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        checkout(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::checkout, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        merge(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::merge, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // upnp

        template<class... OnRequest>
        auto
        msearch(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::msearch, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        notify(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::notify, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        subscribe(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::subscribe, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        unsubscribe(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unsubscribe, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // RFC-5789

        template<class... OnRequest>
        auto
        patch(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::patch, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        purge(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::purge, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // CalDAV

        template<class... OnRequest>
        auto
        mkcalendar(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::mkcalendar, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // RFC-2068, section 19.6.1.2

        template<class... OnRequest>
        auto
        link(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::link, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        unlink(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unlink, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        auto
        all(OnRequest&&... on_request)
        -> decltype (storage_type(std::declval<OnRequest>()...), std::declval<node_type&>())
        {
            router_.add_resource_cb_without_method(router_.tmp_res_regex_, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

    private:

        router_type& router_;

    }; // class chain_node

private:

    inline void
    save_to_res_regex(resource_regex_type const& path_to_resource)
    {
        tmp_res_regex_ = path_to_resource;
    }

    // Temporary variable for storing a regular expression for implementing a chain task of routes
    resource_regex_type tmp_res_regex_;

    std::shared_ptr<resource_map_type> resource_map_;
    std::shared_ptr<method_map_type> method_map_;
}; // class chain_router

} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_CHAIN_ROUTER_HXX
