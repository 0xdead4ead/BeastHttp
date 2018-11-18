#ifndef BEAST_HTTP_LITERAL_HPP
#define BEAST_HTTP_LITERAL_HPP

#include <utility>

namespace http {

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

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... fs) &&{
        instance.get({s_, n_}, std::forward<Callback>(fs)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_get, Param>{*this, std::move(instance)};
    }

};

struct wrap_post{ // POST

    const char * s_;
    std::size_t n_;

    wrap_post(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.post({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_post, Param>{*this, std::move(instance)};
    }

};

struct wrap_put{ // PUT

    const char * s_;
    std::size_t n_;

    wrap_put(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.put({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_put, Param>{*this, std::move(instance)};
    }

};

struct wrap_head{ // HEAD

    const char * s_;
    std::size_t n_;

    wrap_head(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.head({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_head, Param>{*this, std::move(instance)};
    }

};

struct wrap_delete_{ // DELETE

    const char * s_;
    std::size_t n_;

    wrap_delete_(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.delete_({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_delete_, Param>{*this, std::move(instance)};
    }

};

struct wrap_options{ // OPTIONS

    const char * s_;
    std::size_t n_;

    wrap_options(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.options({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_options, Param>{*this, std::move(instance)};
    }

};

struct wrap_connect{ // CONNECT

    const char * s_;
    std::size_t n_;

    wrap_connect(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.connect({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_connect, Param>{*this, std::move(instance)};
    }

};

struct wrap_trace{ // TRACE

    const char * s_;
    std::size_t n_;

    wrap_trace(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.trace({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_trace, Param>{*this, std::move(instance)};
    }

};

struct wrap_copy{ // COPY

    const char * s_;
    std::size_t n_;

    wrap_copy(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.copy({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_copy, Param>{*this, std::move(instance)};
    }

};

struct wrap_lock{ // LOCK

    const char * s_;
    std::size_t n_;

    wrap_lock(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.lock({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_lock, Param>{*this, std::move(instance)};
    }

};

struct wrap_mkcol{ // MKCOL

    const char * s_;
    std::size_t n_;

    wrap_mkcol(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.mkcol({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_mkcol, Param>{*this, std::move(instance)};
    }

};

struct wrap_move{ // MOVE

    const char * s_;
    std::size_t n_;

    wrap_move(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.move({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_move, Param>{*this, std::move(instance)};
    }

};

struct wrap_propfind{ // PROPFIND

    const char * s_;
    std::size_t n_;

    wrap_propfind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.propfind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_propfind, Param>{*this, std::move(instance)};
    }

};

struct wrap_proppatch{ // PROPPATCH

    const char * s_;
    std::size_t n_;

    wrap_proppatch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.proppatch({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_proppatch, Param>{*this, std::move(instance)};
    }

};

struct wrap_search{ // SEARCH

    const char * s_;
    std::size_t n_;

    wrap_search(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.search({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_search, Param>{*this, std::move(instance)};
    }

};

struct wrap_unlock{ // UNLOCK

    const char * s_;
    std::size_t n_;

    wrap_unlock(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.unlock({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_unlock, Param>{*this, std::move(instance)};
    }

};

struct wrap_bind{ // BIND

    const char * s_;
    std::size_t n_;

    wrap_bind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.bind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_bind, Param>{*this, std::move(instance)};
    }

};

struct wrap_rebind{ // REBIND

    const char * s_;
    std::size_t n_;

    wrap_rebind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.rebind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_rebind, Param>{*this, std::move(instance)};
    }

};

struct wrap_unbind{ // UNBIND

    const char * s_;
    std::size_t n_;

    wrap_unbind(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.unbind({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_unbind, Param>{*this, std::move(instance)};
    }

};

struct wrap_acl{ // ACL

    const char * s_;
    std::size_t n_;

    wrap_acl(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.acl({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_acl, Param>{*this, std::move(instance)};
    }

};

struct wrap_report{ // REPORT

    const char * s_;
    std::size_t n_;

    wrap_report(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.report({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_report, Param>{*this, std::move(instance)};
    }

};

struct wrap_mkactivity{ // MKACTIVITY

    const char * s_;
    std::size_t n_;

    wrap_mkactivity(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.mkactivity({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_mkactivity, Param>{*this, std::move(instance)};
    }

};

struct wrap_checkout{ // CHECKOUT

    const char * s_;
    std::size_t n_;

    wrap_checkout(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.checkout({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_checkout, Param>{*this, std::move(instance)};
    }

};

struct wrap_merge{ // MERGE

    const char * s_;
    std::size_t n_;

    wrap_merge(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.merge({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_merge, Param>{*this, std::move(instance)};
    }

};

struct wrap_msearch{ // MSEARCH

    const char * s_;
    std::size_t n_;

    wrap_msearch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.msearch({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_msearch, Param>{*this, std::move(instance)};
    }

};

struct wrap_notify{ // NOTIFY

    const char * s_;
    std::size_t n_;

    wrap_notify(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.notify({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_notify, Param>{*this, std::move(instance)};
    }

};

struct wrap_subscribe{ // SUBSCRIBE

    const char * s_;
    std::size_t n_;

    wrap_subscribe(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.subscribe({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_subscribe, Param>{*this, std::move(instance)};
    }

};

struct wrap_unsubscribe{ // UNSUBSCRIBE

    const char * s_;
    std::size_t n_;

    wrap_unsubscribe(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.options({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_unsubscribe, Param>{*this, std::move(instance)};
    }

};

struct wrap_patch{ // PATCH

    const char * s_;
    std::size_t n_;

    wrap_patch(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.patch({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_patch, Param>{*this, std::move(instance)};
    }

};

struct wrap_purge{ // PURGE

    const char * s_;
    std::size_t n_;

    wrap_purge(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.purge({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_purge, Param>{*this, std::move(instance)};
    }

};

struct wrap_mkcalendar{ // MKCALENDAR

    const char * s_;
    std::size_t n_;

    wrap_mkcalendar(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.mkcalendar({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_mkcalendar, Param>{*this, std::move(instance)};
    }

};

struct wrap_link{ // LINK

    const char * s_;
    std::size_t n_;

    wrap_link(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.link({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_link, Param>{*this, std::move(instance)};
    }

};

struct wrap_unlink{ // UNLINK

    const char * s_;
    std::size_t n_;

    wrap_unlink(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.unlink({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_unlink, Param>{*this, std::move(instance)};
    }

};

struct wrap_all{ // ALL

    const char * s_;
    std::size_t n_;

    wrap_all(const char * s, std::size_t n)
        : s_{s}, n_{n}
    {}

    template<class Server, class... Callback>
    void assign(Server & instance, Callback&&... f){
        instance.all({s_, n_}, std::forward<Callback>(f)...);
    }

    template<class Param>
    auto assign(Param && instance) &&{
        return wrap_param<wrap_all, Param>{*this, std::move(instance)};
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

inline auto operator "" _get(const char* s, std::size_t n){
    return wrap_get{s, n};
}

inline auto operator "" _post(const char* s, std::size_t n){
    return wrap_post{s, n};
}

inline auto operator "" _put(const char* s, std::size_t n){
    return wrap_put{s, n};
}

inline auto operator "" _head(const char* s, std::size_t n){
    return wrap_put{s, n};
}

inline auto operator "" _delete_(const char* s, std::size_t n){
    return wrap_delete_{s, n};
}

inline auto operator "" _options(const char* s, std::size_t n){
    return wrap_options{s, n};
}

inline auto operator "" _connect(const char* s, std::size_t n){
    return wrap_connect{s, n};
}

inline auto operator "" _trace(const char* s, std::size_t n){
    return wrap_trace{s, n};
}

inline auto operator "" _copy(const char* s, std::size_t n){
    return wrap_copy{s, n};
}

inline auto operator "" _lock(const char* s, std::size_t n){
    return wrap_lock{s, n};
}

inline auto operator "" _mkcol(const char* s, std::size_t n){
    return wrap_mkcol{s, n};
}

inline auto operator "" _move(const char* s, std::size_t n){
    return wrap_move{s, n};
}

inline auto operator "" _propfind(const char* s, std::size_t n){
    return wrap_propfind{s, n};
}

inline auto operator "" _proppatch(const char* s, std::size_t n){
    return wrap_proppatch{s, n};
}

inline auto operator "" _search(const char* s, std::size_t n){
    return wrap_search{s, n};
}

inline auto operator "" _unlock(const char* s, std::size_t n){
    return wrap_unlock{s, n};
}

inline auto operator "" _bind(const char* s, std::size_t n){
    return wrap_bind{s, n};
}

inline auto operator "" _rebind(const char* s, std::size_t n){
    return wrap_rebind{s, n};
}

inline auto operator "" _unbind(const char* s, std::size_t n){
    return wrap_unbind{s, n};
}

inline auto operator "" _acl(const char* s, std::size_t n){
    return wrap_acl{s, n};
}

inline auto operator "" _report(const char* s, std::size_t n){
    return wrap_report{s, n};
}

inline auto operator "" _mkactivity(const char* s, std::size_t n){
    return wrap_mkactivity{s, n};
}

inline auto operator "" _checkout(const char* s, std::size_t n){
    return wrap_checkout{s, n};
}

inline auto operator "" _merge(const char* s, std::size_t n){
    return wrap_merge{s, n};
}

inline auto operator "" _msearch(const char* s, std::size_t n){
    return wrap_msearch{s, n};
}

inline auto operator "" _notify(const char* s, std::size_t n){
    return wrap_notify{s, n};
}

inline auto operator "" _subscribe(const char* s, std::size_t n){
    return wrap_subscribe{s, n};
}

inline auto operator "" _unsubscribe(const char* s, std::size_t n){
    return wrap_unsubscribe{s, n};
}

inline auto operator "" _patch(const char* s, std::size_t n){
    return wrap_patch{s, n};
}

inline auto operator "" _purge(const char* s, std::size_t n){
    return wrap_purge{s, n};
}

inline auto operator "" _mkcalendar(const char* s, std::size_t n){
    return wrap_mkcalendar{s, n};
}

inline auto operator "" _link(const char* s, std::size_t n){
    return wrap_link{s, n};
}

inline auto operator "" _unlink(const char* s, std::size_t n){
    return wrap_unlink{s, n};
}

inline auto operator "" _all(const char* s, std::size_t n){
    return wrap_all{s, n};
}

} // namespace literal

} // namespace http

#endif // BEAST_HTTP_LITERAL_HPP
