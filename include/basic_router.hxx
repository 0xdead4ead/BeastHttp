#if not defined BEASTHTTP_BASIC_ROUTER_HXX
#define BEASTHTTP_BASIC_ROUTER_HXX

#include <base/router.hxx>

namespace _0xdead4ead {
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

    using request_type = typename base_type::request_type;

    basic_router() noexcept
        : base_type{resource_map_cb_p_, method_map_cb_p_}
    {}

    template<class... OnRequest>
    auto
    get(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::get, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    post(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::post, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    put(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::put, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    head(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::head, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    delete_(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::delete_, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    options(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::options, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    connect(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::connect, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    trace(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::trace, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // Webdav

    template<class... OnRequest>
    auto
    copy(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::copy, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    lock(resource_regex_type const & path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::lock, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    mkcol(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::mkcol, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    move(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::move, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    propfind(resource_regex_type const& path_to_resource,
             OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::propfind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    proppatch(resource_regex_type const& path_to_resource,
              OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::proppatch, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    search(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::search, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    unlock(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::unlock, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    bind(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::bind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    rebind(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::rebind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    unbind(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::unbind, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    acl(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::acl, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // subversion

    template<class... OnRequest>
    auto
    report(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::report, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    mkactivity(resource_regex_type const& path_to_resource,
               OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::mkactivity, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    checkout(resource_regex_type const& path_to_resource,
             OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::checkout, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    merge(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::merge, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // upnp

    template<class... OnRequest>
    auto
    msearch(resource_regex_type const& path_to_resource,
            OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::msearch, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    notify(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::notify, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    subscribe(resource_regex_type const& path_to_resource,
              OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::subscribe, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    unsubscribe(resource_regex_type const& path_to_resource,
                OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::unsubscribe, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // RFC-5789

    template<class... OnRequest>
    auto
    patch(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::patch, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    purge(resource_regex_type const& path_to_resource,
          OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::purge, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // CalDAV

    template<class... OnRequest>
    auto
    mkcalendar(resource_regex_type const& path_to_resource,
               OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::mkcalendar, storage_type{std::forward<OnRequest>(on_request)...});
    }

    // RFC-2068, section 19.6.1.2

    template<class... OnRequest>
    auto
    link(resource_regex_type const& path_to_resource,
         OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::link, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    unlink(resource_regex_type const& path_to_resource,
           OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
    {
        add_resource_cb(path_to_resource, method_type::unlink, storage_type{std::forward<OnRequest>(on_request)...});
    }

    template<class... OnRequest>
    auto
    all(resource_regex_type const& path_to_resource,
        OnRequest&&... on_request) &
    -> decltype (storage_type(std::declval<OnRequest>()...), void())
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

    template<class Pack>
    auto
    param(typename regex_type::flag_type flags)
    -> decltype (static_cast<base_type*>(this)->template param<self_type, Pack>(*this, flags))
    {
        return base_type::template param<self_type, Pack>(*this, flags);
    }

private:

    std::shared_ptr<resource_map_type> resource_map_cb_p_;
    std::shared_ptr<method_map_type> method_map_cb_p_;

}; // class basic_router

} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASIC_ROUTER_HXX
