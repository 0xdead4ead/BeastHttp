#if not defined BEASTHTTP_BASE_TRAITS_HXX
#define BEASTHTTP_BASE_TRAITS_HXX

#include <type_traits>
#include <utility>

namespace http {
namespace base {
namespace traits {
namespace details {

template<typename...>
using void_t = void;

template<typename F, typename... Args,
         typename = decltype(std::declval<F>()(std::declval<Args&&>()...))>
std::true_type is_invocable_impl(void*);

template<typename, typename...>
std::false_type is_invocable_impl(...);

template<typename F, typename... Args>
struct is_invocable : decltype (is_invocable_impl<F, Args...>(nullptr))
{
}; // struct is_invocable

template <typename F>
struct validity_checker
{
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const
    {
        return is_invocable<F, decltype (args)...>{};
    }
}; // struct validity_checker

} // namespace details

template <typename F>
constexpr auto isValid(F&&)
{
    return details::validity_checker<F&&>{};
}

template<class F, class... Args>
constexpr auto tryInvoke(F&& f, Args&&... args)
{
    return isValid([](auto&& f, auto&&... args)
                   -> decltype (f(std::forward<decltype (args)>(args)...)) {})
            (std::forward<F>(f), std::forward<Args>(args)...);
}

template<class X>
constexpr auto tryCbegin(X&& x)
{
    return isValid([](auto&& x)
                   -> decltype(x.cbegin()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto tryCend(X&& x)
{
    return isValid([](auto&& x)
                   -> decltype(x.cend()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto trySize(X&& x)
{
    return isValid([](auto&& x)
                   -> decltype(x.size()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasConstIterator(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::const_iterator {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasSizeType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::size_type {})
            (std::forward<X>(x));
}

template<class X, class Y>
constexpr auto tryPushBack(X&& x, Y&& y)
{
    return isValid([](auto&& x, auto&& y)
                   -> decltype (x.push_back(std::forward<decltype (y)>(y))){})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class X, class Y, class Z>
constexpr auto tryBind(X&& x, Y&& y, Z&& z)
{
    return isValid([](auto&& x, auto&& y, auto&& z)
                   -> decltype (x.bind(std::forward<decltype (y)>(y), std::forward<decltype (z)>(z))){})
            (std::forward<X>(x), std::forward<Y>(y), std::forward<Z>(z));
}

template<class X>
constexpr auto tryStream(X&& x)
{
    return isValid([](auto&& x)
                   -> decltype(x.stream()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasContextType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::context_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasRequestType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::request_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasFleshType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::flesh_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasDuration(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::duration {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasTimePoint(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::time_point {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasClockType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::clock_type {})
            (std::forward<X>(x));
}

template<class X, class Y>
constexpr auto tryLeftShift(X&& x, Y&& y)
{
    return isValid([](auto&& x, auto&& y)
                   -> decltype (x << y) {})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class X>
constexpr auto hasStorageType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::storage_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceMapType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::resource_map_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasMethodMapType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::method_map_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceRegexType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::resource_regex_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::resource_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasMethodType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::method_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasCbExecutorType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::cbexecutor_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasRegexType(X&& x)
{
    return isValid([](auto&& x)
                   -> typename std::decay_t<decltype (x)>::regex_type {})
            (std::forward<X>(x));
}

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
using HasContextType = decltype (hasContextType(std::declval<X>()));

template<typename X>
using HasRequestType = decltype (hasRequestType(std::declval<X>()));

template<typename X>
using HasFleshType = decltype (hasFleshType(std::declval<X>()));

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
