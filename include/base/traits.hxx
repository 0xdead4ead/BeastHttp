#if not defined BEASTHTTP_BASE_TRAITS_HXX
#define BEASTHTTP_BASE_TRAITS_HXX

#include <type_traits>
#include <utility>

namespace _0xdead4ead {
namespace http {
namespace base {
namespace traits {
namespace details {

template<class...>
using void_t = void;

template<class, class = void_t<>>
struct invoke_result
{
};

template<class F, class... Args>
struct invoke_result<F(Args...), void_t<decltype(
        std::declval<F>()(std::declval<Args>()...))>>
{
  using type = decltype(std::declval<F>()(std::declval<Args>()...));
};

template<class R, class F, class... Args>
auto
is_invocable_impl(F&& f, int, Args&& ...args)
    -> typename std::enable_if<
           std::is_convertible<typename invoke_result<
               decltype (f)(decltype (args)...)>::type, R>::value or
                   std::is_same<R, void>::value, std::true_type>::type;

template<class R, class F, class... Args>
std::false_type
is_invocable_impl(F&&, long, Args&&...);

template<class, class>
struct is_invocable : std::false_type
{
};

template<class F, class R, class... Args>
struct is_invocable<F, R(Args...)>
    : decltype(is_invocable_impl<R>(
                   std::declval<F>(), 1, std::declval<Args>()...))
{
};

namespace typelist {

template<class...>
struct instance
{
};

template<class>
struct front;

template<class Head, class... Tail>
struct front<instance<Head, Tail...>>
{
    using type = Head;
};

template<class>
struct pop_front;

template<class Head, class... Tail>
struct pop_front<instance<Head, Tail...>>
{
    using type = instance<Tail...>;
};

template<unsigned, class, template<class, unsigned> class>
struct for_each;

template<unsigned Index, template<class, unsigned> class Pred, class... Elements>
struct for_each<Index, instance<Elements...>, Pred>
        : Pred<typename front<instance<Elements...>>::type, Index>,
        for_each<Index + 1, typename pop_front<instance<Elements...>>::type, Pred>
{
};

template<unsigned Index, template<class, unsigned> class Pred>
struct for_each<Index, instance<>, Pred>
{
};

template<class, unsigned>
struct get;

template<unsigned Index, class... Elements>
struct get<instance<Elements...>, Index>
        : get<typename pop_front<instance<Elements...>>::type, Index - 1>
{
};

template<class... Elements>
struct get<instance<Elements...>, 0>
        : front<instance<Elements...>>
{
};

} // namespace typelist

template<class...>
struct conjunction;

template<>
struct conjunction<> : std::true_type
{
};

template<class B1>
struct conjunction<B1> : B1
{
};

template<class B1, class B2>
struct conjunction<B1, B2>
        : std::conditional<B1::value, B2, B1>::type
{
};

template<class B1, class B2, class B3, class... Bn>
struct conjunction<B1, B2, B3, Bn...>
        : std::conditional<B1::value, conjunction<B2, B3, Bn...>, B1>::type
{
};

#if not defined __cpp_generic_lambdas
#define BEASTHTTP_CXX11_TRAITS

template<class R, class F, class... Args>
struct try_invoke_cxx11 : is_invocable<F, R(Args...)>
{
};

struct try_cbegin_helper
{
    template<class X>
    auto operator()(X&&) -> decltype (std::declval<X>().cbegin());
};

template<class R, class X>
struct try_cbegin_cxx11 : try_invoke_cxx11<R, try_cbegin_helper, X>
{
};

struct try_cend_helper
{
    template<class X>
    auto operator()(X&&) -> decltype (std::declval<X>().cend());
};

template<class R, class X>
struct try_cend_cxx11 : try_invoke_cxx11<R, try_cend_helper, X>
{
};

struct try_find_helper
{
    template<class X, class Y>
    auto operator()(X&&, Y&&) -> decltype (std::declval<X>().find(std::declval<Y>()));
};

template<class R, class X, class Y>
struct try_find_cxx11 : try_invoke_cxx11<R, try_find_helper, X, Y>
{
};

struct try_size_helper
{
    template<class X>
    auto operator()(X&&) -> decltype (std::declval<X>().size());
};

template<class R, class X>
struct try_size_cxx11 : try_invoke_cxx11<R, try_size_helper, X>
{
};

struct has_const_iterator_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::const_iterator;
};

template<class R, class X>
struct has_const_iterator_cxx11 : try_invoke_cxx11<R, has_const_iterator_helper, X>
{
};

struct has_size_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::size_type;
};

template<class R, class X>
struct has_size_type_cxx11 : try_invoke_cxx11<R, has_size_type_helper, X>
{
};

struct try_push_back_helper
{
    template<class X, class Y>
    auto operator()(X&&, Y&&) -> decltype (std::declval<X>().push_back(std::declval<Y>()));
};

template<class R, class X, class Y>
struct try_push_back_cxx11 : try_invoke_cxx11<R, try_push_back_helper, X, Y>
{
};

struct try_bind_helper
{
    template<class X, class Y, class Z>
    auto operator()(X&&, Y&&, Z&&) -> decltype (std::declval<X>().bind(std::declval<Y>(), std::declval<Z>()));
};

template<class R, class X, class Y, class Z>
struct try_bind_cxx11 : try_invoke_cxx11<R, try_bind_helper, X, Y, Z>
{
};

struct try_stream_helper
{
    template<class X>
    auto operator()(X&&) -> decltype (std::declval<X>().stream());
};

template<class R, class X>
struct try_stream_cxx11 : try_invoke_cxx11<R, try_stream_helper, X>
{
};

struct has_context_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::context_type;
};

template<class R, class X>
struct has_context_type_cxx11 : try_invoke_cxx11<R, has_context_type_helper, X>
{
};

struct has_request_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::request_type;
};

template<class R, class X>
struct has_request_type_cxx11 : try_invoke_cxx11<R, has_request_type_helper, X>
{
};

struct has_flesh_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::flesh_type;
};

template<class R, class X>
struct has_flesh_type_cxx11 : try_invoke_cxx11<R, has_flesh_type_helper, X>
{
};

struct has_duration_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::duration;
};

template<class R, class X>
struct has_duration_cxx11 : try_invoke_cxx11<R, has_duration_helper, X>
{
};

struct has_time_point_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::time_point;
};

template<class R, class X>
struct has_time_point_cxx11 : try_invoke_cxx11<R, has_time_point_helper, X>
{
};

struct has_clock_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::clock_type;
};

template<class R, class X>
struct has_clock_type_cxx11 : try_invoke_cxx11<R, has_clock_type_helper, X>
{
};

struct try_leftshift_helper
{
    template<class X, class Y>
    auto operator()(X&&, Y&&) -> decltype (std::declval<X>() << std::declval<Y>());
};

template<class R, class X, class Y>
struct try_leftshift_cxx11 : try_invoke_cxx11<R, try_leftshift_helper, X, Y>
{
};

struct has_storage_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::storage_type;
};

template<class R, class X>
struct has_storage_type_cxx11 : try_invoke_cxx11<R, has_storage_type_helper, X>
{
};

struct has_resource_map_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::resource_map_type;
};

template<class R, class X>
struct has_resource_map_type_cxx11 : try_invoke_cxx11<R, has_resource_map_type_helper, X>
{
};

struct has_method_map_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::method_map_type;
};

template<class R, class X>
struct has_method_map_type_cxx11 : try_invoke_cxx11<R, has_method_map_type_helper, X>
{
};

struct has_resource_regex_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::resource_regex_type;
};

template<class R, class X>
struct has_resource_regex_type_cxx11 : try_invoke_cxx11<R, has_resource_regex_type_helper, X>
{
};

struct has_resource_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::resource_type;
};

template<class R, class X>
struct has_resource_type_cxx11 : try_invoke_cxx11<R, has_resource_type_helper, X>
{
};

struct has_method_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::method_type;
};

template<class R, class X>
struct has_method_type_cxx11 : try_invoke_cxx11<R, has_method_type_helper, X>
{
};

struct has_cbexecutor_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::cbexecutor_type;
};

template<class R, class X>
struct has_cbexecutor_type_cxx11 : try_invoke_cxx11<R, has_cbexecutor_type_helper, X>
{
};

struct has_regex_type_helper
{
    template<class X>
    auto operator()(X&&) -> typename std::decay<X>::type::regex_type;
};

template<class R, class X>
struct has_regex_type_cxx11 : try_invoke_cxx11<R, has_regex_type_helper, X>
{
};
#else
template <class R, class F>
struct validity_checker
{
    template <class... Args>
    constexpr auto operator()(Args&&... args) const
    {
        return is_invocable<F, R(decltype (args)...)>{};
    }
};

template <class R, class F>
constexpr auto isValid(F&&)
{
    return validity_checker<R, F>{};
}

#endif // not defined __cpp_generic_lambdas
} // namespace details

#if not defined BEASTHTTP_CXX11_TRAITS
template<class R, class F, class... Args>
constexpr auto tryInvoke(F&& f, Args&&... args)
{
    return details::isValid<R>(
                [](auto&& f, auto&&... args)
                -> decltype (f(std::forward<decltype (args)>(args)...)) {})
            (std::forward<F>(f), std::forward<Args>(args)...);
}

template<class R, class X>
constexpr auto tryCbegin(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> decltype(x.cbegin()){})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto tryCend(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> decltype(x.cend()){})
            (std::forward<X>(x));
}

template<class R, class X, class Y>
constexpr auto tryFind(X&& x, Y&& y)
{
    return details::isValid<R>(
                [](auto&& x, auto&& y) -> decltype(x.find(std::forward<decltype (y)>(y))){})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class R, class X>
constexpr auto trySize(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> decltype(x.size()){})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasConstIterator(X&& x)
{
    return details::isValid<void>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::const_iterator {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasSizeType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::size_type {})
            (std::forward<X>(x));
}

template<class R, class X, class Y>
constexpr auto tryPushBack(X&& x, Y&& y)
{
    return details::isValid<R>(
                [](auto&& x, auto&& y) -> decltype (x.push_back(std::forward<decltype (y)>(y))){})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class R, class X, class Y, class Z>
constexpr auto tryBind(X&& x, Y&& y, Z&& z)
{
    return details::isValid<R>(
                [](auto&& x, auto&& y, auto&& z) -> decltype (x.bind(std::forward<decltype (y)>(y), std::forward<decltype (z)>(z))){})
            (std::forward<X>(x), std::forward<Y>(y), std::forward<Z>(z));
}

template<class R, class X>
constexpr auto tryStream(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> decltype(x.stream()){})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasContextType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::context_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasRequestType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::request_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasFleshType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::flesh_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasDuration(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::duration {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasTimePoint(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::time_point {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasClockType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::clock_type {})
            (std::forward<X>(x));
}

template<class R, class X, class Y>
constexpr auto tryLeftShift(X&& x, Y&& y)
{
    return details::isValid<R>(
                [](auto&& x, auto&& y) -> decltype (x << y) {})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class R, class X>
constexpr auto hasStorageType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::storage_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasResourceMapType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::resource_map_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasMethodMapType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::method_map_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasResourceRegexType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::resource_regex_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasResourceType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::resource_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasMethodType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::method_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasCbExecutorType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::cbexecutor_type {})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasRegexType(X&& x)
{
    return details::isValid<R>(
                [](auto&& x) -> typename std::decay_t<decltype (x)>::regex_type {})
            (std::forward<X>(x));
}
#else
template<class R, class F, class... Args>
constexpr auto tryInvoke(F&&, Args&&...) -> decltype (
        details::try_invoke_cxx11<R, F, Args...>{});

template<class R, class X>
constexpr auto tryCbegin(X&&) -> decltype (
        details::try_cbegin_cxx11<R, X>{});

template<class R, class X>
constexpr auto tryCend(X&&) -> decltype (
        details::try_cend_cxx11<R, X>{});

template<class R, class X, class Y>
constexpr auto tryFind(X&&, Y&&)
-> decltype (details::try_find_cxx11<R, X, Y>{});

template<class R, class X>
constexpr auto trySize(X&&)
-> decltype (details::try_size_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasConstIterator(X&&)
-> decltype (details::has_const_iterator_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasSizeType(X&&)
-> decltype (details::has_size_type_cxx11<R, X>{});

template<class R, class X, class Y>
constexpr auto tryPushBack(X&&, Y&&)
-> decltype (details::try_push_back_cxx11<R, X, Y>{});

template<class R, class X, class Y, class Z>
constexpr auto tryBind(X&&, Y&&, Z&&)
-> decltype (details::try_bind_cxx11<R, X, Y, Z>{});

template<class R, class X>
constexpr auto tryStream(X&&)
-> decltype (details::try_stream_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasContextType(X&&)
-> decltype (details::has_context_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasRequestType(X&&)
-> decltype (details::has_request_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasFleshType(X&&)
-> decltype (details::has_flesh_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasDuration(X&&)
-> decltype (details::has_duration_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasTimePoint(X&&)
-> decltype (details::has_time_point_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasClockType(X&&)
-> decltype (details::has_clock_type_cxx11<R, X>{});

template<class R, class X, class Y>
constexpr auto tryLeftShift(X&&, Y&&)
-> decltype (details::try_leftshift_cxx11<R, X, Y>{});

template<class R, class X>
constexpr auto hasStorageType(X&&)
-> decltype (details::has_storage_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasResourceMapType(X&&)
-> decltype (details::has_resource_map_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasMethodMapType(X&&)
-> decltype (details::has_method_map_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasResourceRegexType(X&&)
-> decltype (details::has_resource_regex_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasResourceType(X&&)
-> decltype (details::has_resource_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasMethodType(X&&)
-> decltype (details::has_method_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasCbExecutorType(X&&)
-> decltype (details::has_cbexecutor_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasRegexType(X&&)
-> decltype (details::has_regex_type_cxx11<R, X>{});
#endif // not defined BEASTHTTP_CXX11_TRAITS

template<class, class>
struct TryInvoke;

template<class F, class R, class... Args>
struct TryInvoke<F, R(Args...)>
    : decltype (tryInvoke<R>(std::declval<F>(), std::declval<Args>()...))
{
};

template<unsigned FnCount, class Sig1, class Sig2, class F, class... Fn>
struct TryInvokeConjunction
        : TryInvokeConjunction<FnCount - 1, Sig1, Sig2, Fn...>
{
    static constexpr bool value =
            TryInvoke<F, Sig1>::value and TryInvokeConjunction<FnCount - 1, Sig1, Sig2, Fn...>::value;
};

template<class Sig1, class Sig2, class F>
struct TryInvokeConjunction<0, Sig1, Sig2, F>
{
    static constexpr bool value = TryInvoke<F, Sig2>::value;
};

template<class, class>
struct TryCbegin;

template<class R, class X>
struct TryCbegin<X, R()>
    : decltype (tryCbegin<R>(std::declval<X>()))
{
};

template<class, class>
struct TryCend;

template<class R, class X>
struct TryCend<X, R()>
    : decltype (tryCend<R>(std::declval<X>()))
{
};

template<class, class>
struct TryFind;

template<class R, class X, class Y>
struct TryFind<X, R(Y)>
    : decltype (tryFind<R>(std::declval<X>(), std::declval<Y>()))
{
};

template<class X, class R>
using HasConstIterator = decltype (hasConstIterator<R>(std::declval<X>()));

template<class X, class R>
using HasSizeType = decltype (hasSizeType<R>(std::declval<X>()));

template<class, class>
struct TrySize;

template<class R, class X>
struct TrySize<X, R()>
    : decltype (trySize<R>(std::declval<X>()))
{
};

template<class, class>
struct TryPushBack;

template<class R, class X, class Y>
struct TryPushBack<X, R(Y)>
    : decltype (tryPushBack<R>(std::declval<X>(), std::declval<Y>()))
{
};

template<class, class>
struct TryBind;

template<class R, class X, class Y, class Z>
struct TryBind<X, R(Y, Z)>
    : decltype (tryBind<R>(std::declval<X>(), std::declval<Y>(), std::declval<Z>()))
{
};

template<class, class>
struct TryStream;

template<class R, class X>
struct TryStream<X, R()>
    : decltype (tryStream<R>(std::declval<X>()))
{
};

template<class X, class R>
using HasContextType = decltype (hasContextType<R>(std::declval<X>()));

template<class X, class R>
using HasRequestType = decltype (hasRequestType<R>(std::declval<X>()));

template<class X, class R>
using HasFleshType = decltype (hasFleshType<R>(std::declval<X>()));

template<class X, class R>
using HasDuration = decltype (hasDuration<R>(std::declval<X>()));

template<class X, class R>
using HasTimePoint = decltype (hasTimePoint<R>(std::declval<X>()));

template<class X, class R>
using HasClockType = decltype (hasClockType<R>(std::declval<X>()));

template<class, class>
struct TryLeftShift;

template<class R, class X, class Y>
struct TryLeftShift<X, R(Y)>
    : decltype (tryLeftShift<R>(std::declval<X>(), std::declval<Y>()))
{
};

template<class X, class R>
using HasStorageType = decltype (hasStorageType<R>(std::declval<X>()));

template<class X, class R>
using HasResourceMapType = decltype (hasResourceMapType<R>(std::declval<X>()));

template<class X, class R>
using HasMethodMapType = decltype (hasMethodMapType<R>(std::declval<X>()));

template<class X, class R>
using HasResourceRegexType = decltype (hasResourceRegexType<R>(std::declval<X>()));

template<class X, class R>
using HasResourceType = decltype (hasResourceType<R>(std::declval<X>()));

template<class X, class R>
using HasMethodType = decltype (hasMethodType<R>(std::declval<X>()));

template<class X, class R>
using HasCbExecutorType = decltype (hasCbExecutorType<R>(std::declval<X>()));

template<class X, class R>
using HasRegexType = decltype (hasRegexType<R>(std::declval<X>()));

template<class... Elements>
using TypeList = details::typelist::instance<Elements...>;

template<class TypeList, template<class, unsigned> class Pred>
using ForEach = details::typelist::for_each<0, TypeList, Pred>;

template<class TypeList, unsigned Index>
using Get = details::typelist::get<TypeList, Index>;

template<class... Bn>
using Conjunction = details::conjunction<Bn...>;

} // namespace traits
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_TRAITS_HXX
