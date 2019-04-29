#if not defined BEASTHTTP_LITERALS_HXX
#define BEASTHTTP_LITERALS_HXX

#include <cassert>
#include <cstddef>
#include <tuple>

#if __cplusplus >= 201402L
#define BEASTHTTP_LITERALS_DETECT_CXX14
#endif

#define BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(x) class x;

#define BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(x) \
    template<> \
    struct __verb<symbol:: x>{ \
        const char* s_; \
        std::size_t n_; \
        template<class Router, class... OnRequest> \
        auto advance(Router& router, OnRequest&&... on_request) && \
        -> decltype (router. x({s_, n_}, std::forward<OnRequest>(on_request)...)) \
        { \
            return router. x({s_, n_}, std::forward<OnRequest>(on_request)...); \
        } \
        template<class Param, class... OnRequest> \
        auto advance(Param&& param, OnRequest&&... on_request) && \
        -> decltype (std::move(param). x({s_, n_}, std::forward<OnRequest>(on_request)...)) \
        { \
            return std::move(param). x({s_, n_}, std::forward<OnRequest>(on_request)...); \
        } \
    };

#define BEASTHTTP_LITERALS_DECLARE_ROUTE_STRUCT \
    struct __route \
    { \
        const char* s_; \
        std::size_t n_; \
        template<class Router> \
        auto advance(Router& router) && -> decltype (router.route({s_, n_})) { \
            return router.route({s_, n_}); \
        } \
        template<class Param> \
        auto advance(Param&& param) && -> decltype (std::move(param).route({s_, n_})) { \
            return std::move(param).route({s_, n_}); \
        } \
    };

namespace _0xdead4ead {
namespace http {
namespace literals {
namespace detail {

#if defined BEASTHTTP_LITERALS_DETECT_CXX14
constexpr int toInt(char c)
{
    if (c >= 'A' and c <= 'F') {
        return static_cast<int>(c) - static_cast<int>('A') + 10;
    }
    if (c >= 'a' and c <= 'f') {
        return static_cast<int>(c) - static_cast<int>('a') + 10;
    }
    assert(c >= '0' and c <= '9');
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
#endif // defined BEASTHTTP_LITERALS_DETECT_CXX14

namespace symbol {

BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(get)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(post)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(put)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(head)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(delete_)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(options)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(connect)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(trace)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(copy)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(lock)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(mkcol)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(move)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(propfind)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(proppatch)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(search)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(unlock)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(bind)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(rebind)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(unbind)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(acl)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(report)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(mkactivity)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(checkout)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(merge)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(msearch)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(notify)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(subscribe)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(unsubscribe)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(patch)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(purge)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(mkcalendar)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(link)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(unlink)
BEASTHTTP_LITERALS_DECLARE_VERB_SYMBOL(all)

} // namespace symbol

template<class> struct __verb;
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(get)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(post)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(put)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(head)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(delete_)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(options)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(connect)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(trace)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(copy)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(lock)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(mkcol)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(move)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(propfind)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(proppatch)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(search)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(unlock)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(bind)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(rebind)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(unbind)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(acl)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(report)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(mkactivity)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(checkout)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(merge)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(msearch)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(notify)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(subscribe)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(unsubscribe)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(patch)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(purge)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(mkcalendar)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(link)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(unlink)
BEASTHTTP_LITERALS_DECLARE_VERB_STRUCT(all)

BEASTHTTP_LITERALS_DECLARE_ROUTE_STRUCT

} // namespace detail

#if defined BEASTHTTP_LITERALS_DETECT_CXX14
template<char... cs>
constexpr auto operator"" _c()
{
  return std::integral_constant<int, detail::parseInt<sizeof...(cs)>({cs...})>{};
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
#endif // defined BEASTHTTP_LITERALS_DETECT_CXX14

inline detail::__verb<detail::symbol::get>
operator "" _get(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::get>{s, n};
}

inline detail::__verb<detail::symbol::post>
operator "" _post(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::post>{s, n};
}

inline detail::__verb<detail::symbol::put>
operator "" _put(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::put>{s, n};
}

inline detail::__verb<detail::symbol::head>
operator "" _head(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::head>{s, n};
}

inline detail::__verb<detail::symbol::delete_>
operator "" _delete(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::delete_>{s, n};
}

inline detail::__verb<detail::symbol::options>
operator "" _options(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::options>{s, n};
}

inline detail::__verb<detail::symbol::connect>
operator "" _connect(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::connect>{s, n};
}

inline detail::__verb<detail::symbol::trace>
operator "" _trace(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::trace>{s, n};
}

inline detail::__verb<detail::symbol::copy>
operator "" _copy(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::copy>{s, n};
}

inline detail::__verb<detail::symbol::lock>
operator "" _lock(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::lock>{s, n};
}

inline detail::__verb<detail::symbol::mkcol>
operator "" _mkcol(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::mkcol>{s, n};
}

inline detail::__verb<detail::symbol::move>
operator "" _move(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::move>{s, n};
}

inline detail::__verb<detail::symbol::propfind>
operator "" _propfind(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::propfind>{s, n};
}

inline detail::__verb<detail::symbol::proppatch>
operator "" _proppatch(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::proppatch>{s, n};
}

inline detail::__verb<detail::symbol::search>
operator "" _search(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::search>{s, n};
}

inline detail::__verb<detail::symbol::unlock>
operator "" _unlock(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::unlock>{s, n};
}

inline detail::__verb<detail::symbol::bind>
operator "" _bind(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::bind>{s, n};
}

inline detail::__verb<detail::symbol::rebind>
operator "" _rebind(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::rebind>{s, n};
}

inline detail::__verb<detail::symbol::unbind>
operator "" _unbind(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::unbind>{s, n};
}

inline detail::__verb<detail::symbol::acl>
operator "" _acl(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::acl>{s, n};
}

inline detail::__verb<detail::symbol::report>
operator "" _report(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::report>{s, n};
}

inline detail::__verb<detail::symbol::mkactivity>
operator "" _mkactivity(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::mkactivity>{s, n};
}

inline detail::__verb<detail::symbol::checkout>
operator "" _checkout(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::checkout>{s, n};
}

inline detail::__verb<detail::symbol::merge>
operator "" _merge(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::merge>{s, n};
}

inline detail::__verb<detail::symbol::msearch>
operator "" _msearch(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::msearch>{s, n};
}

inline detail::__verb<detail::symbol::notify>
operator "" _notify(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::notify>{s, n};
}

inline detail::__verb<detail::symbol::subscribe>
operator "" _subscribe(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::subscribe>{s, n};
}

inline detail::__verb<detail::symbol::unsubscribe>
operator "" _unsubscribe(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::unsubscribe>{s, n};
}

inline detail::__verb<detail::symbol::patch>
operator "" _patch(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::patch>{s, n};
}

inline detail::__verb<detail::symbol::purge>
operator "" _purge(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::purge>{s, n};
}

inline detail::__verb<detail::symbol::mkcalendar>
operator "" _mkcalendar(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::mkcalendar>{s, n};
}

inline detail::__verb<detail::symbol::link>
operator "" _link(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::link>{s, n};
}

inline detail::__verb<detail::symbol::unlink>
operator "" _unlink(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::unlink>{s, n};
}

inline detail::__verb<detail::symbol::all>
operator "" _all(const char* s, std::size_t n)
{
    return detail::__verb<detail::symbol::all>{s, n};
}

inline detail::__route
operator "" _route(const char* s, std::size_t n)
{
    return detail::__route{s, n};
}

} // namespace literals
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_LITERALS_HXX
