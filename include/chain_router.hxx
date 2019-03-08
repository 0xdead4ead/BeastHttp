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
#if defined BEASTHTTP_PARAM_HXX
    template<class... Types>
    auto
    param(typename regex_type::flag_type flags)
    -> param::impl<self_type, param::pack<Types...>>
    {
        return base_type::template param<self_type, param::pack<Types...>>(*this, flags);
    }
#endif
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
        node_type&
        get(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::get, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        post(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::post, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        put(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::put, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        head(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::head, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        delete_(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::delete_, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        options(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::options, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        connect(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::connect, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        trace(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::trace, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        //Webdav

        template<class... OnRequest>
        node_type&
        copy(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::copy, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        lock(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::lock, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        mkcol(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::mkcol, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        move(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::move, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        propfind(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::propfind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        proppatch(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::proppatch, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        search(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::search, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        unlock(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unlock, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        bind(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::bind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        rebind(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::rebind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        unbind(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unbind, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        acl(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::acl, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // subversion

        template<class... OnRequest>
        node_type&
        report(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::report, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        mkactivity(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::mkactivity, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        checkout(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::checkout, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        merge(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::merge, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // upnp

        template<class... OnRequest>
        node_type&
        msearch(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::msearch, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        notify(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::notify, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        subscribe(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::subscribe, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        unsubscribe(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unsubscribe, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // RFC-5789

        template<class... OnRequest>
        node_type&
        patch(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::patch, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        purge(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::purge, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // CalDAV

        template<class... OnRequest>
        node_type&
        mkcalendar(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::mkcalendar, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        // RFC-2068, section 19.6.1.2

        template<class... OnRequest>
        node_type&
        link(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::link, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        unlink(OnRequest&&... on_request)
        {
            router_.add_resource_cb(router_.tmp_res_regex_, method_type::unlink, storage_type{std::forward<OnRequest>(on_request)...});
            return *this;
        }

        template<class... OnRequest>
        node_type&
        all(OnRequest&&... on_request)
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
