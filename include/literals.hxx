#if not defined BEASTHTTP_LITERALS_HXX
#define BEASTHTTP_LITERALS_HXX

#include <cassert>
#include <cstddef>
#include <tuple>

namespace http {
namespace literals {

constexpr int toInt(char c)
{
  if (c >= 'A' && c <= 'F') {
    return static_cast<int>(c) - static_cast<int>('A') + 10;
  }
  if (c >= 'a' && c <= 'f') {
    return static_cast<int>(c) - static_cast<int>('a') + 10;
  }
  assert(c >= '0' && c <= '9');
  return static_cast<int>(c) - static_cast<int>('0');
}

template<std::size_t N>
constexpr int parseInt(char const (&array)[N])
{
  int base = 10;
  int offset = 0;
  if (N > 2 && array[0] == '0') {
    switch (array[1]) {
      case 'x':
      case 'X':
        base = 16;
        offset = 2;
        break;
      case 'b':
      case 'B':
        base = 2;
        offset = 2;
        break;
      default:
        base = 8;
        offset = 1;
        break;
    }
  }
  int value = 0;
  int multiplier = 1;
  for (std::size_t i = 0; i < N - offset; ++i) {
    if (array[N-1-i] != '\'') {
      value += toInt(array[N-1-i]) * multiplier;
      multiplier *= base;
    }
  }
  return value;
}

template<char... cs>
constexpr auto operator"" _c()
{
  return std::integral_constant<int, parseInt<sizeof...(cs)>({cs...})>{};
}

template<int Index, class... Types>
auto&
value(std::tuple<Types...>& tpl, std::integral_constant<int, Index>)
{
    return std::get<Index>(tpl);
}

template<int Index, class... Types>
auto&
value(const std::tuple<Types...>& tpl, std::integral_constant<int, Index>)
{
    return std::get<Index>(tpl);
}

class get;
class post;
class put;
class head;
class delete_;
class options;
class connect;
class trace;
class copy;
class lock;
class mkcol;
class move;
class propfind;
class proppatch;
class search;
class unlock;
class bind;
class rebind;
class unbind;
class acl;
class report;
class mkactivity;
class checkout;
class merge;
class msearch;
class notify;
class subscribe;
class unsubscribe;
class patch;
class purge;
class mkcalendar;
class link;
class unlink;
class all;

template<class>
struct _verb;

#define BEASTHTTP_LITERALS_DECLARE_VERB(x) \
    template<> \
    struct _verb<x>{ \
        const char* s_; \
        std::size_t n_; \
        template<class Router, class... OnRequest> \
        auto advance(Router& router, OnRequest&&... on_request) && \
        -> decltype (router.x({s_, n_}, std::forward<OnRequest>(on_request)...)) { \
            router.x({s_, n_}, std::forward<OnRequest>(on_request)...); \
        } \
        template<class Param> \
        auto advance(Param&& param) && { \
            return _param<_verb<x>, std::decay_t<Param>>{*this, std::move(param)}; \
        } \
    };

#define BEASTHTTP_LITERALS_PARAM_WITH(x) \
    template<class... OnRequest> \
    auto \
    with(OnRequest&&... on_request) && \
    -> decltype (std::enable_if_t<std::is_same<Verb, _verb<x>>::value, int>{}, \
                 std::move(param_).x({verb_.s_, verb_.n_}, std::forward<OnRequest>(on_request)...)) { \
        std::move(param_).x({verb_.s_, verb_.n_}, std::forward<OnRequest>(on_request)...); \
    }

template<class Verb, class Param>
struct _param
{
    Verb& verb_;
    Param&& param_;

    BEASTHTTP_LITERALS_PARAM_WITH(get)
    BEASTHTTP_LITERALS_PARAM_WITH(post)
    BEASTHTTP_LITERALS_PARAM_WITH(put)
    BEASTHTTP_LITERALS_PARAM_WITH(head)
    BEASTHTTP_LITERALS_PARAM_WITH(delete_)
    BEASTHTTP_LITERALS_PARAM_WITH(options)
    BEASTHTTP_LITERALS_PARAM_WITH(connect)
    BEASTHTTP_LITERALS_PARAM_WITH(trace)
    BEASTHTTP_LITERALS_PARAM_WITH(copy)
    BEASTHTTP_LITERALS_PARAM_WITH(lock)
    BEASTHTTP_LITERALS_PARAM_WITH(mkcol)
    BEASTHTTP_LITERALS_PARAM_WITH(move)
    BEASTHTTP_LITERALS_PARAM_WITH(propfind)
    BEASTHTTP_LITERALS_PARAM_WITH(proppatch)
    BEASTHTTP_LITERALS_PARAM_WITH(search)
    BEASTHTTP_LITERALS_PARAM_WITH(unlock)
    BEASTHTTP_LITERALS_PARAM_WITH(bind)
    BEASTHTTP_LITERALS_PARAM_WITH(rebind)
    BEASTHTTP_LITERALS_PARAM_WITH(unbind)
    BEASTHTTP_LITERALS_PARAM_WITH(acl)
    BEASTHTTP_LITERALS_PARAM_WITH(report)
    BEASTHTTP_LITERALS_PARAM_WITH(mkactivity)
    BEASTHTTP_LITERALS_PARAM_WITH(checkout)
    BEASTHTTP_LITERALS_PARAM_WITH(merge)
    BEASTHTTP_LITERALS_PARAM_WITH(msearch)
    BEASTHTTP_LITERALS_PARAM_WITH(notify)
    BEASTHTTP_LITERALS_PARAM_WITH(subscribe)
    BEASTHTTP_LITERALS_PARAM_WITH(unsubscribe)
    BEASTHTTP_LITERALS_PARAM_WITH(patch)
    BEASTHTTP_LITERALS_PARAM_WITH(purge)
    BEASTHTTP_LITERALS_PARAM_WITH(mkcalendar)
    BEASTHTTP_LITERALS_PARAM_WITH(link)
    BEASTHTTP_LITERALS_PARAM_WITH(unlink)
    BEASTHTTP_LITERALS_PARAM_WITH(all)

};

BEASTHTTP_LITERALS_DECLARE_VERB(get)
BEASTHTTP_LITERALS_DECLARE_VERB(post)
BEASTHTTP_LITERALS_DECLARE_VERB(put)
BEASTHTTP_LITERALS_DECLARE_VERB(head)
BEASTHTTP_LITERALS_DECLARE_VERB(delete_)
BEASTHTTP_LITERALS_DECLARE_VERB(options)
BEASTHTTP_LITERALS_DECLARE_VERB(connect)
BEASTHTTP_LITERALS_DECLARE_VERB(trace)
BEASTHTTP_LITERALS_DECLARE_VERB(copy)
BEASTHTTP_LITERALS_DECLARE_VERB(lock)
BEASTHTTP_LITERALS_DECLARE_VERB(mkcol)
BEASTHTTP_LITERALS_DECLARE_VERB(move)
BEASTHTTP_LITERALS_DECLARE_VERB(propfind)
BEASTHTTP_LITERALS_DECLARE_VERB(proppatch)
BEASTHTTP_LITERALS_DECLARE_VERB(search)
BEASTHTTP_LITERALS_DECLARE_VERB(unlock)
BEASTHTTP_LITERALS_DECLARE_VERB(bind)
BEASTHTTP_LITERALS_DECLARE_VERB(rebind)
BEASTHTTP_LITERALS_DECLARE_VERB(unbind)
BEASTHTTP_LITERALS_DECLARE_VERB(acl)
BEASTHTTP_LITERALS_DECLARE_VERB(report)
BEASTHTTP_LITERALS_DECLARE_VERB(mkactivity)
BEASTHTTP_LITERALS_DECLARE_VERB(checkout)
BEASTHTTP_LITERALS_DECLARE_VERB(merge)
BEASTHTTP_LITERALS_DECLARE_VERB(msearch)
BEASTHTTP_LITERALS_DECLARE_VERB(notify)
BEASTHTTP_LITERALS_DECLARE_VERB(subscribe)
BEASTHTTP_LITERALS_DECLARE_VERB(unsubscribe)
BEASTHTTP_LITERALS_DECLARE_VERB(patch)
BEASTHTTP_LITERALS_DECLARE_VERB(purge)
BEASTHTTP_LITERALS_DECLARE_VERB(mkcalendar)
BEASTHTTP_LITERALS_DECLARE_VERB(link)
BEASTHTTP_LITERALS_DECLARE_VERB(unlink)
BEASTHTTP_LITERALS_DECLARE_VERB(all)

inline auto operator "" _get(const char* s, std::size_t n){
    return _verb<get>{s, n};
}

inline auto operator "" _post(const char* s, std::size_t n){
    return _verb<post>{s, n};
}

inline auto operator "" _put(const char* s, std::size_t n){
    return _verb<put>{s, n};
}

inline auto operator "" _head(const char* s, std::size_t n){
    return _verb<head>{s, n};
}

inline auto operator "" _delete(const char* s, std::size_t n){
    return _verb<delete_>{s, n};
}

inline auto operator "" _options(const char* s, std::size_t n){
    return _verb<options>{s, n};
}

inline auto operator "" _connect(const char* s, std::size_t n){
    return _verb<connect>{s, n};
}

inline auto operator "" _trace(const char* s, std::size_t n){
    return _verb<trace>{s, n};
}

inline auto operator "" _copy(const char* s, std::size_t n){
    return _verb<copy>{s, n};
}

inline auto operator "" _lock(const char* s, std::size_t n){
    return _verb<lock>{s, n};
}

inline auto operator "" _mkcol(const char* s, std::size_t n){
    return _verb<mkcol>{s, n};
}

inline auto operator "" _move(const char* s, std::size_t n){
    return _verb<move>{s, n};
}

inline auto operator "" _propfind(const char* s, std::size_t n){
    return _verb<propfind>{s, n};
}

inline auto operator "" _proppatch(const char* s, std::size_t n){
    return _verb<proppatch>{s, n};
}

inline auto operator "" _search(const char* s, std::size_t n){
    return _verb<search>{s, n};
}

inline auto operator "" _unlock(const char* s, std::size_t n){
    return _verb<unlock>{s, n};
}

inline auto operator "" _bind(const char* s, std::size_t n){
    return _verb<bind>{s, n};
}

inline auto operator "" _rebind(const char* s, std::size_t n){
    return _verb<rebind>{s, n};
}

inline auto operator "" _unbind(const char* s, std::size_t n){
    return _verb<unbind>{s, n};
}

inline auto operator "" _acl(const char* s, std::size_t n){
    return _verb<acl>{s, n};
}

inline auto operator "" _report(const char* s, std::size_t n){
    return _verb<report>{s, n};
}

inline auto operator "" _mkactivity(const char* s, std::size_t n){
    return _verb<mkactivity>{s, n};
}

inline auto operator "" _checkout(const char* s, std::size_t n){
    return _verb<checkout>{s, n};
}

inline auto operator "" _merge(const char* s, std::size_t n){
    return _verb<merge>{s, n};
}

inline auto operator "" _msearch(const char* s, std::size_t n){
    return _verb<msearch>{s, n};
}

inline auto operator "" _notify(const char* s, std::size_t n){
    return _verb<notify>{s, n};
}

inline auto operator "" _subscribe(const char* s, std::size_t n){
    return _verb<subscribe>{s, n};
}

inline auto operator "" _unsubscribe(const char* s, std::size_t n){
    return _verb<unsubscribe>{s, n};
}

inline auto operator "" _patch(const char* s, std::size_t n){
    return _verb<patch>{s, n};
}

inline auto operator "" _purge(const char* s, std::size_t n){
    return _verb<purge>{s, n};
}

inline auto operator "" _mkcalendar(const char* s, std::size_t n){
    return _verb<mkcalendar>{s, n};
}

inline auto operator "" _link(const char* s, std::size_t n){
    return _verb<link>{s, n};
}

inline auto operator "" _unlink(const char* s, std::size_t n){
    return _verb<unlink>{s, n};
}

inline auto operator "" _all(const char* s, std::size_t n){
    return _verb<all>{s, n};
}

} // namespace literals
} // namespace http

#endif // not defined BEASTHTTP_LITERALS_HXX
