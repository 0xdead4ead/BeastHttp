#if not defined BEASTHTTP_BASE_TRAITS_HXX
#define BEASTHTTP_BASE_TRAITS_HXX

#include <type_traits>
#include <utility>

namespace http {
namespace base {
namespace traits {
namespace details {

#if not defined __cpp_generic_lambdas
#define BEASTHTTP_CXX11_TRAITS
template<typename...>
using void_t = void;

template <class, class = details::void_t<>>
struct try_invoke_cxx11 : std::false_type
{
};

template <class F, class... Args>
struct try_invoke_cxx11<F(Args...), details::void_t<decltype (
        std::declval<F>()(std::declval<Args>()...))>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct try_cbegin_cxx11 : std::false_type
{
};

template<class X>
struct try_cbegin_cxx11<X, details::void_t<decltype (
        std::declval<X>().cbegin())>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct try_cend_cxx11 : std::false_type
{
};

template<class X>
struct try_cend_cxx11<X, details::void_t<decltype (
        std::declval<X>().cend())>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct try_size_cxx11 : std::false_type
{
};

template<class X>
struct try_size_cxx11<X, details::void_t<decltype (
        std::declval<X>().size())>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_const_iterator_cxx11 : std::false_type
{
};

template<class X>
struct has_const_iterator_cxx11<X, details::void_t<
        typename X::const_iterator>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_size_type_cxx11 : std::false_type
{
};

template<class X>
struct has_size_type_cxx11<X, details::void_t<
        typename X::size_type>> : std::true_type
{
};

template<class, class, class = details::void_t<>>
struct try_push_back_cxx11 : std::false_type
{
};

template<class X, class Y>
struct try_push_back_cxx11<X, Y, details::void_t<decltype (
        std::declval<X>().push_back(std::declval<Y>()))>> : std::true_type
{
};

template<class, class, class, class = details::void_t<>>
struct try_bind_cxx11 : std::false_type
{
};

template<class X, class Y, class Z>
struct try_bind_cxx11<X, Y, Z, details::void_t<decltype (
        std::declval<X>().bind(std::declval<Y>(), std::declval<Z>()))>> : std::true_type
{
};

template<class X, class = details::void_t<>>
struct try_stream_cxx11 : std::false_type
{
};

template<class X>
struct try_stream_cxx11<X, details::void_t<decltype (
        std::declval<X>().stream())>> : std::true_type
{
};

template<class X, class = details::void_t<>>
struct has_context_type_cxx11 : std::false_type
{
};

template<class X>
struct has_context_type_cxx11<X, details::void_t<
        typename X::context_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_request_type_cxx11 : std::false_type
{
};

template<class X>
struct has_request_type_cxx11<X, details::void_t<
        typename X::request_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_flesh_type_cxx11 : std::false_type
{
};

template<class X>
struct has_flesh_type_cxx11<X, details::void_t<
        typename X::flesh_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_duration_cxx11 : std::false_type
{
};

template<class X>
struct has_duration_cxx11<X, details::void_t<
        typename X::duration>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_time_point_cxx11 : std::false_type
{
};

template<class X>
struct has_time_point_cxx11<X, details::void_t<
        typename X::time_point>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_clock_type_cxx11 : std::false_type
{
};

template<class X>
struct has_clock_type_cxx11<X, details::void_t<
        typename X::clock_type>> : std::true_type
{
};

template<class, class, class = details::void_t<>>
struct try_leftshift_cxx11 : std::false_type
{
};

template<class X, class Y>
struct try_leftshift_cxx11<X, Y, details::void_t<decltype (
        std::declval<X>() << std::declval<Y>())>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_storage_type_cxx11 : std::false_type
{
};

template<class X>
struct has_storage_type_cxx11<X, details::void_t<
        typename X::storage_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_resource_map_type_cxx11 : std::false_type
{
};

template<class X>
struct has_resource_map_type_cxx11<X, details::void_t<
        typename X::resource_map_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_method_map_type_cxx11 : std::false_type
{
};

template<class X>
struct has_method_map_type_cxx11<X, details::void_t<
        typename X::method_map_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_resource_regex_type_cxx11 : std::false_type
{
};

template<class X>
struct has_resource_regex_type_cxx11<X, details::void_t<
        typename X::resource_regex_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_resource_type_cxx11 : std::false_type
{
};

template<class X>
struct has_resource_type_cxx11<X, details::void_t<
        typename X::resource_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_method_type_cxx11 : std::false_type
{
};

template<class X>
struct has_method_type_cxx11<X, details::void_t<
        typename X::method_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_cbexecutor_type_cxx11 : std::false_type
{
};

template<class X>
struct has_cbexecutor_type_cxx11<X, details::void_t<
        typename X::cbexecutor_type>> : std::true_type
{
};

template<class, class = details::void_t<>>
struct has_regex_type_cxx11 : std::false_type
{
};

template<class X>
struct has_regex_type_cxx11<X, details::void_t<
        typename X::regex_type>> : std::true_type
{
};
#else
template<typename F, typename... Args,
         typename = decltype(std::declval<F>()(std::declval<Args&&>()...))>
std::true_type is_invocable_impl(void*);

template<typename, typename...>
std::false_type is_invocable_impl(...);

template<typename F, typename... Args>
struct is_invocable : decltype (is_invocable_impl<F, Args...>(nullptr))
{
};

template <typename F>
struct validity_checker
{
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const
    {
        return is_invocable<F, decltype (args)...>{};
    }
};

template <typename F>
constexpr auto isValid(F&&)
{
    return validity_checker<F&&>{};
}

#endif
} // namespace details

#if not defined BEASTHTTP_CXX11_TRAITS
template<class F, class... Args>
constexpr auto tryInvoke(F&& f, Args&&... args)
{
    return details::isValid([](auto&& f, auto&&... args)
                            -> decltype (f(std::forward<decltype (args)>(args)...)) {})
            (std::forward<F>(f), std::forward<Args>(args)...);
}

template<class X>
constexpr auto tryCbegin(X&& x)
{
    return details::isValid([](auto&& x)
                            -> decltype(x.cbegin()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto tryCend(X&& x)
{
    return details::isValid([](auto&& x)
                            -> decltype(x.cend()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto trySize(X&& x)
{
    return details::isValid([](auto&& x)
                            -> decltype(x.size()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasConstIterator(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::const_iterator {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasSizeType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::size_type {})
            (std::forward<X>(x));
}

template<class X, class Y>
constexpr auto tryPushBack(X&& x, Y&& y)
{
    return details::isValid([](auto&& x, auto&& y)
                            -> decltype (x.push_back(std::forward<decltype (y)>(y))){})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class X, class Y, class Z>
constexpr auto tryBind(X&& x, Y&& y, Z&& z)
{
    return details::isValid([](auto&& x, auto&& y, auto&& z)
                            -> decltype (x.bind(std::forward<decltype (y)>(y), std::forward<decltype (z)>(z))){})
            (std::forward<X>(x), std::forward<Y>(y), std::forward<Z>(z));
}

template<class X>
constexpr auto tryStream(X&& x)
{
    return details::isValid([](auto&& x)
                            -> decltype(x.stream()){})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasContextType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::context_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasRequestType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::request_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasFleshType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::flesh_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasDuration(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::duration {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasTimePoint(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::time_point {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasClockType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::clock_type {})
            (std::forward<X>(x));
}

template<class X, class Y>
constexpr auto tryLeftShift(X&& x, Y&& y)
{
    return details::isValid([](auto&& x, auto&& y)
                            -> decltype (x << y) {})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class X>
constexpr auto hasStorageType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::storage_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceMapType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::resource_map_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasMethodMapType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::method_map_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceRegexType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::resource_regex_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasResourceType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::resource_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasMethodType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::method_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasCbExecutorType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::cbexecutor_type {})
            (std::forward<X>(x));
}

template<class X>
constexpr auto hasRegexType(X&& x)
{
    return details::isValid([](auto&& x)
                            -> typename std::decay_t<decltype (x)>::regex_type {})
            (std::forward<X>(x));
}
#else
template<class F, class... Args>
constexpr auto tryInvoke(F&&, Args&&...)
-> decltype (details::try_invoke_cxx11<F(Args...)>{});

template<class X>
constexpr auto tryCbegin(X&&)
-> decltype (details::try_cbegin_cxx11<X>{});

template<class X>
constexpr auto tryCend(X&&)
-> decltype (details::try_cend_cxx11<X>{});

template<class X>
constexpr auto trySize(X&&)
-> decltype (details::try_size_cxx11<X>{});

template<class X>
constexpr auto hasConstIterator(X&&)
-> decltype (details::has_const_iterator_cxx11<X>{});

template<class X>
constexpr auto hasSizeType(X&&)
-> decltype (details::has_size_type_cxx11<X>{});

template<class X, class Y>
constexpr auto tryPushBack(X&&, Y&&)
-> decltype (details::try_push_back_cxx11<X, Y>{});

template<class X, class Y, class Z>
constexpr auto tryBind(X&&, Y&&, Z&&)
-> decltype (details::try_bind_cxx11<X, Y, Z>{});

template<class X>
constexpr auto tryStream(X&&)
-> decltype (details::try_stream_cxx11<X>{});

template<class X>
constexpr auto hasContextType(X&&)
-> decltype (details::has_context_type_cxx11<X>{});

template<class X>
constexpr auto hasRequestType(X&&)
-> decltype (details::has_request_type_cxx11<X>{});

template<class X>
constexpr auto hasFleshType(X&&)
-> decltype (details::has_flesh_type_cxx11<X>{});

template<class X>
constexpr auto hasDuration(X&&)
-> decltype (details::has_duration_cxx11<X>{});

template<class X>
constexpr auto hasTimePoint(X&&)
-> decltype (details::has_time_point_cxx11<X>{});

template<class X>
constexpr auto hasClockType(X&&)
-> decltype (details::has_clock_type_cxx11<X>{});

template<class X, class Y>
constexpr auto tryLeftShift(X&&, Y&&)
-> decltype (details::try_leftshift_cxx11<X, Y>{});

template<class X>
constexpr auto hasStorageType(X&&)
-> decltype (details::has_storage_type_cxx11<X>{});

template<class X>
constexpr auto hasResourceMapType(X&&)
-> decltype (details::has_resource_map_type_cxx11<X>{});

template<class X>
constexpr auto hasMethodMapType(X&&)
-> decltype (details::has_method_map_type_cxx11<X>{});

template<class X>
constexpr auto hasResourceRegexType(X&&)
-> decltype (details::has_resource_regex_type_cxx11<X>{});

template<class X>
constexpr auto hasResourceType(X&&)
-> decltype (details::has_resource_type_cxx11<X>{});

template<class X>
constexpr auto hasMethodType(X&&)
-> decltype (details::has_method_type_cxx11<X>{});

template<class X>
constexpr auto hasCbExecutorType(X&&)
-> decltype (details::has_cbexecutor_type_cxx11<X>{});

template<class X>
constexpr auto hasRegexType(X&&)
-> decltype (details::has_regex_type_cxx11<X>{});
#endif

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
