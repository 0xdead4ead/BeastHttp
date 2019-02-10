#if not defined BEASTHTTP_BASE_TRAITS_HXX
#define BEASTHTTP_BASE_TRAITS_HXX

#include <type_traits>
#include <bits/move.h>

namespace std {

#if not defined __cpp_lib_void_t
template<typename...>
using void_t = void;
#endif // not defined __cpp_lib_void_t

#if not defined __cpp_lib_is_invocable

template<typename F, typename... Args, typename = std::result_of_t<F(Args...)>>
std::true_type is_invocable_impl(void*);

template<typename F, typename... Args>
std::false_type is_invocable_impl(...);

/**
  Not used
*/
template <typename, typename = std::void_t<>>
struct is_invocable : std::false_type { };

template <typename F, class... Args>
struct is_invocable<F(Args...), std::void_t<std::result_of_t<F(Args...)>>>
    : std::true_type { };

#endif // not defined __cpp_lib_is_invocable

} // namespace std

namespace http {
namespace base {
namespace traits {

#if not defined __cpp_inline_variables
template <typename F>
struct validity_checker
{
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const
    {
        return std::is_invocable_impl<F, decltype (args)...>(nullptr);
    }
};

template<class T>
class Debug;

template <typename F>
constexpr auto isValid(F&&)
{
    return validity_checker<F&&>{};
}

template<class F, class... Args>
constexpr auto tryInvoke(F&& f, Args&&... args)
{
    return isValid([](auto&& f, auto&&... args) -> decltype (f(std::forward<decltype (args)>(args)...)) {})
            (std::forward<F>(f), std::forward<Args>(args)...);
}

template<class X>
constexpr auto tryCbegin(X&& x)
{
    return isValid([](auto&& x) -> decltype(x.cbegin()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto tryCend(X&& x)
{
    return isValid([](auto&& x) -> decltype(x.cend()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto trySize(X&& x)
{
    return isValid([](auto&& x) -> decltype(x.size()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasConstIterator(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::const_iterator {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasSizeType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::size_type {})
            (std::forward<X>(x));
}

template<class X, class Y>
constexpr auto tryPushBack(X&& x, Y&& y)
{
    return isValid([](auto&& x, auto&& y) -> decltype (x.push_back(std::forward<decltype (y)>(y))){})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class X, class Y, class Z>
constexpr auto tryBind(X&& x, Y&& y, Z&& z)
{
    return isValid([](auto&& x, auto&& y, auto&& z) -> decltype (x.bind(std::forward<decltype (y)>(y), std::forward<decltype (z)>(z))){})
            (std::forward<X>(x), std::forward<Y>(y), std::forward<Z>(z));
}

template<class X>
constexpr auto tryStream(X&& x)
{
    return isValid([](auto&& x) -> decltype(x.stream()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasContext(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::context {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasRequestType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::request_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasFlesh(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::flesh {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasDuration(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::duration {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasTimePoint(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::time_point {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasClockType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::clock_type {})
            (std::forward<X>(x));
}

template<class X, class Y>
constexpr auto tryLeftShift(X&& x, Y&& y)
{
    return isValid([](auto&& x, auto&& y) -> decltype (x << y) {})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class X>
constexpr auto hasStorageType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::storage_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceMapType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::resource_map_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasMethodMapType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::method_map_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceRegexType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::resource_regex_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::resource_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasMethodType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::method_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasCbExecutorType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::cbexecutor_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasRegexType(X&& x)
{
    return isValid([](auto&& x) -> typename std::decay_t<decltype (x)>::regex_type {})
            (std::forward<X>(x));
}
#else
static inline constexpr auto isValid = [](auto&& f) {
    return [](auto&&... args) {
        return decltype (std::is_invocable<decltype(f), decltype(args)...>{}){};
    };
};

static inline constexpr auto tryInvoke
    = isValid([](auto&& f, auto&&... args)
              -> decltype (f(std::forward<decltype (args)>(args) ... )) {});

static inline constexpr auto tryCbegin
    = isValid([](auto&& x) -> decltype (x.cbegin()) {});

static inline constexpr auto tryCend
    = isValid([](auto&& x) -> decltype (x.cend()) {});

static inline constexpr auto trySize
    = isValid([](auto&& x) -> decltype (x.size()) {});

static inline constexpr auto hasConstIterator
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::const_iterator {});

static inline constexpr auto hasSizeType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::size_type {});

static inline constexpr auto tryPushBack
    = isValid([](auto&& x, auto&& y) -> decltype (x.push_back(std::forward<decltype (y)>(y))) {});

static inline constexpr auto tryBind
    = isValid([](auto&& x, auto&& y, auto&& z) -> decltype (x.bind(std::forward<decltype (y)>(y), std::forward<decltype (z)>(z))) {});

static inline constexpr auto tryStream
    = isValid([](auto&& x) -> decltype (x.stream()) {});

static inline constexpr auto hasContext
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::context {});

static inline constexpr auto hasRequestType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::request_type {});

static inline constexpr auto hasFlesh
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::flesh {});

static inline constexpr auto hasDuration
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::duration {});

static inline constexpr auto hasTimePoint
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::time_point {});

static inline constexpr auto hasClockType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::clock_type {});

static inline constexpr auto tryLeftShift
    = isValid([](auto&& x, auto&& y) -> decltype (x << y) {});

static inline constexpr auto hasStorageType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::storage_type {});

static inline constexpr auto hasResourceMapType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::resource_map_type {});

static inline constexpr auto hasMethodMapType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::method_map_type {});

static inline constexpr auto hasResourceRegexType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::resource_regex_type {});

static inline constexpr auto hasResourceType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::resource_type {});

static inline constexpr auto hasMethodType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::method_type {});

static inline constexpr auto hasCbExecutorType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::cbexecutor_type {});

static inline constexpr auto hasRegexType
    = isValid([](auto&& x)
              -> typename std::decay_t<decltype (x)>::regex_type {});
#endif // not defined __cpp_inline_variables

template<class F, class... Args>
using TryInvoke = decltype (tryInvoke(std::declval<F>(), std::declval<Args>()...));

template<typename X>
using TryCbegin = decltype (tryCbegin(std::declval<X>()));

template<typename X>
using TryCend = decltype (tryCend(std::declval<X>()));

template<typename X>
using HasConstIterator = decltype (hasConstIterator(std::declval<X>()));

template<typename X>
using HasSizeType = decltype (hasSizeType(std::declval<X>()));

template<typename X>
using TrySize = decltype (trySize(std::declval<X>()));

template<typename X, typename Y>
using TryPushBack = decltype (tryPushBack(std::declval<X>(), std::declval<Y>()));

template<typename X, typename Y, typename Z>
using TryBind = decltype (tryBind(std::declval<X>(), std::declval<Y>(), std::declval<Z>()));

template<typename X>
using TryStream = decltype (tryStream(std::declval<X>()));

template<typename X>
using HasContext = decltype (hasContext(std::declval<X>()));

template<typename X>
using HasRequestType = decltype (hasRequestType(std::declval<X>()));

template<typename X>
using HasFlesh = decltype (hasFlesh(std::declval<X>()));

template<typename X>
using HasDuration = decltype (hasDuration(std::declval<X>()));

template<typename X>
using HasTimePoint = decltype (hasTimePoint(std::declval<X>()));

template<typename X>
using HasClockType = decltype (hasClockType(std::declval<X>()));

template<typename X, typename Y>
using TryLeftShift = decltype (tryLeftShift(std::declval<X>(), std::declval<Y>()));

template<typename X>
using HasStorageType = decltype (hasStorageType(std::declval<X>()));

template<typename X>
using HasResourceMapType = decltype (hasResourceMapType(std::declval<X>()));

template<typename X>
using HasMethodMapType = decltype (hasMethodMapType(std::declval<X>()));

template<typename X>
using HasResourceRegexType = decltype (hasResourceRegexType(std::declval<X>()));

template<typename X>
using HasResourceType = decltype (hasResourceType(std::declval<X>()));

template<typename X>
using HasMethodType = decltype (hasMethodType(std::declval<X>()));

template<typename X>
using HasCbExecutorType = decltype (hasCbExecutorType(std::declval<X>()));

template<typename X>
using HasRegexType = decltype (hasRegexType(std::declval<X>()));

} // namespace traits
} // namespace base
} // namespace http

#endif // not defined BEASTHTTP_BASE_TRAITS_HXX
