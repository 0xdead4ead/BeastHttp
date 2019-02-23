#if not defined BEASTHTTP_BASIC_ROUTER_HXX
#define BEASTHTTP_BASIC_ROUTER_HXX

#include "base/router.hxx"

namespace http {

template<class Session>
class basic_router : public base::router<Session>
{
    using base_type = base::router<Session>;

    using self_type = basic_router;

    using base_type::add_resource_cb;

    using base_type::add_resource_cb_without_method;

public:

    using session_type = typename base_type::session_type;

    using storage_type = typename base_type::storage_type;

    using resource_map_type = typename base_type::resource_map_type;

    using method_map_type = typename base_type::method_map_type;

    using method_type = typename base_type::method_type;

    using resource_regex_type = typename base_type::resource_regex_type;

    using regex_type = typename base_type::regex_type;

    basic_router() noexcept
        : base_type{resource_map_cb_p_, method_map_cb_p_}
    {}

    basic_router(const self_type& other) noexcept
        : resource_map_cb_p_{other.resource_map_cb_p_},
          method_map_cb_p_{other.method_map_cb_p_}
    {}

    self_type&
    operator=(const self_type& other) noexcept
    {
        resource_map_cb_p_ = other.resource_map_cb_p_;
        method_map_cb_p_ = other.method_map_cb_p_;
        return *this;
    }

    basic_router(self_type&& other) noexcept
        : resource_map_cb_p_{std::move(other.resource_map_cb_p_)},
          method_map_cb_p_{std::move(other.method_map_cb_p_)}
    {}

    self_type& operator=(self_type&& other) noexcept
    {
        resource_map_cb_p_ = std::move(other.resource_map_cb_p_);
        method_map_cb_p_ = std::move(other.method_map_cb_p_);
        return *this;
    }

    template<class... OnRequest>
    void
    get(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::get, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    post(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::post, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    put(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::put, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    head(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::head, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    delete_(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::delete_, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    options(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::options, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    connect(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::connect, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    trace(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::trace, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // Webdav

    template<class... OnRequest>
    void
    copy(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::copy, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    lock(resource_regex_type const & path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::lock, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    mkcol(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::mkcol, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    move(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::move, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    propfind(resource_regex_type const& path_to_resource,
             OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::propfind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    proppatch(resource_regex_type const& path_to_resource,
              OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::proppatch, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    search(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::search, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    unlock(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::unlock, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    bind(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::bind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    rebind(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::rebind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    unbind(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::unbind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    acl(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::acl, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // subversion

    template<class... OnRequest>
    void
    report(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::report, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    mkactivity(resource_regex_type const& path_to_resource,
               OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::mkactivity, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    checkout(resource_regex_type const& path_to_resource,
             OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::checkout, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    merge(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::merge, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // upnp

    template<class... OnRequest>
    void
    msearch(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::msearch, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    notify(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::notify, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    subscribe(resource_regex_type const& path_to_resource,
              OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::subscribe, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    unsubscribe(resource_regex_type const& path_to_resource,
                OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::unsubscribe, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // RFC-5789

    template<class... OnRequest>
    void
    patch(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::patch, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    purge(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::purge, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // CalDAV

    template<class... OnRequest>
    void
    mkcalendar(resource_regex_type const& path_to_resource,
               OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::mkcalendar, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // RFC-2068, section 19.6.1.2

    template<class... OnRequest>
    void
    link(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::link, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    unlink(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    {
        add_resource_cb(path_to_resource, method_type::unlink, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    void
    all(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    {
        add_resource_cb_without_method(path_to_resource, storage_type{std::forward<OnRequest>(on_request)...});
    }

    void
    use(resource_regex_type const& path_to_resource,
        base_type const& other)
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

    std::shared_ptr<resource_map_type> resource_map_cb_p_;
    std::shared_ptr<method_map_type> method_map_cb_p_;

}; // class basic_router

} // namespace http

#endif // not defined BEASTHTTP_BASIC_ROUTER_HXX
