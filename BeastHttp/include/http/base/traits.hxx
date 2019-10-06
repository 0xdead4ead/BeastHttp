#if not defined BEASTHTTP_BASE_TRAITS_HXX
#define BEASTHTTP_BASE_TRAITS_HXX

#include <http/base/config.hxx>

#include <type_traits>
#include <utility>

namespace _0xdead4ead {
namespace http {
namespace base {
namespace traits {
namespace detail {

template<class T>
struct wrap
{
    using type = T;
};

template<class T>
T value(wrap<T>);

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

#if not defined BEASTHTTP_CXX14_GENERIC_LAMBDAS
template<class R, class F, class... Args>
struct try_invoke_cxx11 : is_invocable<F, R(Args...)>
{
};

struct has_const_iterator_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::const_iterator;
};

template<class R, class X>
struct has_const_iterator_cxx11 : try_invoke_cxx11<R, has_const_iterator_helper, X>
{
};

struct has_size_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::size_type;
};

template<class R, class X>
struct has_size_type_cxx11 : try_invoke_cxx11<R, has_size_type_helper, X>
{
};

struct try_stream_helper
{
    template<class X>
    auto operator()(X&& x) -> decltype (x.stream());
};

template<class R, class X>
struct try_stream_cxx11 : try_invoke_cxx11<R, try_stream_helper, X>
{
};

struct has_context_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::context_type;
};

template<class R, class X>
struct has_context_type_cxx11 : try_invoke_cxx11<R, has_context_type_helper, X>
{
};

struct has_request_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::request_type;
};

template<class R, class X>
struct has_request_type_cxx11 : try_invoke_cxx11<R, has_request_type_helper, X>
{
};

struct has_flesh_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::flesh_type;
};

template<class R, class X>
struct has_flesh_type_cxx11 : try_invoke_cxx11<R, has_flesh_type_helper, X>
{
};

struct has_duration_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::duration;
};

template<class R, class X>
struct has_duration_cxx11 : try_invoke_cxx11<R, has_duration_helper, X>
{
};

struct has_time_point_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::time_point;
};

template<class R, class X>
struct has_time_point_cxx11 : try_invoke_cxx11<R, has_time_point_helper, X>
{
};

struct has_clock_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::clock_type;
};

template<class R, class X>
struct has_clock_type_cxx11 : try_invoke_cxx11<R, has_clock_type_helper, X>
{
};

struct try_leftshift_helper
{
    template<class X, class Y>
    auto operator()(X&& x, Y&& y) -> decltype (x << y);
};

template<class R, class X, class Y>
struct try_leftshift_cxx11 : try_invoke_cxx11<R, try_leftshift_helper, X, Y>
{
};

struct has_storage_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::storage_type;
};

template<class R, class X>
struct has_storage_type_cxx11 : try_invoke_cxx11<R, has_storage_type_helper, X>
{
};

struct has_resource_map_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::resource_map_type;
};

template<class R, class X>
struct has_resource_map_type_cxx11 : try_invoke_cxx11<R, has_resource_map_type_helper, X>
{
};

struct has_method_map_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::method_map_type;
};

template<class R, class X>
struct has_method_map_type_cxx11 : try_invoke_cxx11<R, has_method_map_type_helper, X>
{
};

struct has_resource_regex_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::resource_regex_type;
};

template<class R, class X>
struct has_resource_regex_type_cxx11 : try_invoke_cxx11<R, has_resource_regex_type_helper, X>
{
};

struct has_resource_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::resource_type;
};

template<class R, class X>
struct has_resource_type_cxx11 : try_invoke_cxx11<R, has_resource_type_helper, X>
{
};

struct has_method_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::method_type;
};

template<class R, class X>
struct has_method_type_cxx11 : try_invoke_cxx11<R, has_method_type_helper, X>
{
};

struct has_cbexecutor_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::cbexecutor_type;
};

template<class R, class X>
struct has_cbexecutor_type_cxx11 : try_invoke_cxx11<R, has_cbexecutor_type_helper, X>
{
};

struct has_regex_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::regex_type;
};

template<class R, class X>
struct has_regex_type_cxx11 : try_invoke_cxx11<R, has_regex_type_helper, X>
{
};

struct has_regex_flag_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::regex_flag_type;
};

template<class R, class X>
struct has_regex_flag_type_cxx11 : try_invoke_cxx11<R, has_regex_flag_type_helper, X>
{
};

struct has_value_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::value_type;
};

template<class R, class X>
struct has_value_type_cxx11 : try_invoke_cxx11<R, has_value_type_helper, X>
{
};

struct has_session_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::session_type;
};

template<class R, class X>
struct has_session_type_cxx11 : try_invoke_cxx11<R, has_session_type_helper, X>
{
};

struct has_iterator_type_helper
{
    template<class X>
    auto operator()(wrap<X> x) -> typename decltype (value(x))::iterator_type;
};

template<class R, class X>
struct has_iterator_type_cxx11 : try_invoke_cxx11<R, has_iterator_type_helper, X>
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
#endif // BEASTHTTP_CXX14_GENERIC_LAMBDAS

#if defined BEASTHTTP_CXX14_GENERIC_LAMBDAS
template<class R, class F, class... Args>
constexpr auto tryInvoke(F&& f, Args&&... args)
{
    return isValid<R>([](auto&& f, auto&&... args)
                      -> decltype (f(std::forward<decltype (args)>(args)...)) {})
            (std::forward<F>(f), std::forward<Args>(args)...);
}

template<class R, class X>
constexpr auto hasConstIterator(wrap<X> x)
{
    return isValid<void>(
                [](auto x) -> typename decltype (value(x))::const_iterator {})(x);
}

template<class R, class X>
constexpr auto hasSizeType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::size_type {})(x);
}

template<class R, class X>
constexpr auto tryStream(X&& x)
{
    return isValid<R>(
                [](auto&& x) -> decltype(x.stream()){})
            (std::forward<X>(x));
}

template<class R, class X>
constexpr auto hasContextType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::context_type {})(x);
}

template<class R, class X>
constexpr auto hasRequestType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::request_type {})(x);
}

template<class R, class X>
constexpr auto hasFleshType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::flesh_type {})(x);
}

template<class R, class X>
constexpr auto hasDuration(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::duration {})(x);
}

template<class R, class X>
constexpr auto hasTimePoint(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::time_point {})(x);
}

template<class R, class X>
constexpr auto hasClockType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::clock_type {})(x);
}

template<class R, class X, class Y>
constexpr auto tryLeftShift(X&& x, Y&& y)
{
    return isValid<R>(
                [](auto&& x, auto&& y) -> decltype (x << y) {})
            (std::forward<X>(x), std::forward<Y>(y));
}

template<class R, class X>
constexpr auto hasStorageType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::storage_type {})(x);
}

template<class R, class X>
constexpr auto hasResourceMapType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::resource_map_type {})(x);
}

template<class R, class X>
constexpr auto hasMethodMapType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::method_map_type {})(x);
}

template<class R, class X>
constexpr auto hasResourceRegexType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::resource_regex_type {})(x);
}

template<class R, class X>
constexpr auto hasResourceType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::resource_type {})(x);
}

template<class R, class X>
constexpr auto hasMethodType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::method_type {})(x);
}

template<class R, class X>
constexpr auto hasCbExecutorType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::cbexecutor_type {})(x);
}

template<class R, class X>
constexpr auto hasRegexType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::regex_type {})(x);
}

template<class R, class X>
constexpr auto hasRegexFlagType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::regex_flag_type {})(x);
}

template<class R, class X>
constexpr auto hasValueType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::value_type {})(x);
}

template<class R, class X>
constexpr auto hasSessionType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::session_type {})(x);
}

template<class R, class X>
constexpr auto hasIteratorType(wrap<X> x)
{
    return isValid<R>(
                [](auto x) -> typename decltype (value(x))::iterator_type {})(x);
}
#else
template<class R, class F, class... Args>
constexpr auto tryInvoke(F&&, Args&&...)
-> decltype (try_invoke_cxx11<R, F, Args...>{});

template<class R, class X>
constexpr auto hasConstIterator(X&&)
-> decltype (has_const_iterator_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasSizeType(X&&)
-> decltype (has_size_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto tryStream(X&&)
-> decltype (try_stream_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasContextType(X&&)
-> decltype (has_context_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasRequestType(X&&)
-> decltype (has_request_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasFleshType(X&&)
-> decltype (has_flesh_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasDuration(X&&)
-> decltype (has_duration_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasTimePoint(X&&)
-> decltype (has_time_point_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasClockType(X&&)
-> decltype (has_clock_type_cxx11<R, X>{});

template<class R, class X, class Y>
constexpr auto tryLeftShift(X&&, Y&&)
-> decltype (try_leftshift_cxx11<R, X, Y>{});

template<class R, class X>
constexpr auto hasStorageType(X&&)
-> decltype (has_storage_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasResourceMapType(X&&)
-> decltype (has_resource_map_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasMethodMapType(X&&)
-> decltype (has_method_map_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasResourceRegexType(X&&)
-> decltype (has_resource_regex_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasResourceType(X&&)
-> decltype (has_resource_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasMethodType(X&&)
-> decltype (has_method_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasCbExecutorType(X&&)
-> decltype (has_cbexecutor_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasRegexType(X&&)
-> decltype (has_regex_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasRegexFlagType(X&&)
-> decltype (has_regex_flag_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasValueType(X&&)
-> decltype (has_value_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasSessionType(X&&)
-> decltype (has_session_type_cxx11<R, X>{});

template<class R, class X>
constexpr auto hasIteratorType(X&&)
-> decltype (has_iterator_type_cxx11<R, X>{});
#endif // BEASTHTTP_CXX14_GENERIC_LAMBDAS
} // namespace detail

template<class, class>
struct TryInvoke;

template<class F, class R, class... Args>
struct TryInvoke<F, R(Args...)>
    : decltype (detail::tryInvoke<R>(std::declval<F>(), std::declval<Args>()...))
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

template<class X, class R>
using HasConstIterator = decltype (detail::hasConstIterator<R>(detail::wrap<X>()));

template<class X, class R>
using HasSizeType = decltype (detail::hasSizeType<R>(detail::wrap<X>()));

template<class, class>
struct TryStream;

template<class R, class X>
struct TryStream<X, R()>
    : decltype (detail::tryStream<R>(std::declval<X>()))
{
};

template<class X, class R>
using HasContextType = decltype (detail::hasContextType<R>(detail::wrap<X>()));

template<class X, class R>
using HasRequestType = decltype (detail::hasRequestType<R>(detail::wrap<X>()));

template<class X, class R>
using HasFleshType = decltype (detail::hasFleshType<R>(detail::wrap<X>()));

template<class X, class R>
using HasDuration = decltype (detail::hasDuration<R>(detail::wrap<X>()));

template<class X, class R>
using HasTimePoint = decltype (detail::hasTimePoint<R>(detail::wrap<X>()));

template<class X, class R>
using HasClockType = decltype (detail::hasClockType<R>(detail::wrap<X>()));

template<class, class>
struct TryLeftShift;

template<class R, class X, class Y>
struct TryLeftShift<X, R(Y)>
    : decltype (detail::tryLeftShift<R>(std::declval<X>(), std::declval<Y>()))
{
};

template<class X, class R>
using HasStorageType = decltype (detail::hasStorageType<R>(detail::wrap<X>()));

template<class X, class R>
using HasResourceMapType = decltype (detail::hasResourceMapType<R>(detail::wrap<X>()));

template<class X, class R>
using HasMethodMapType = decltype (detail::hasMethodMapType<R>(detail::wrap<X>()));

template<class X, class R>
using HasResourceRegexType = decltype (detail::hasResourceRegexType<R>(detail::wrap<X>()));

template<class X, class R>
using HasResourceType = decltype (detail::hasResourceType<R>(detail::wrap<X>()));

template<class X, class R>
using HasMethodType = decltype (detail::hasMethodType<R>(detail::wrap<X>()));

template<class X, class R>
using HasCbExecutorType = decltype (detail::hasCbExecutorType<R>(detail::wrap<X>()));

template<class X, class R>
using HasRegexType = decltype (detail::hasRegexType<R>(detail::wrap<X>()));

template<class X, class R>
using HasRegexFlagType = decltype (detail::hasRegexFlagType<R>(detail::wrap<X>()));

template<class... Elements>
using TypeList = detail::typelist::instance<Elements...>;

template<class TypeList, template<class, unsigned> class Pred>
using ForEach = detail::typelist::for_each<0, TypeList, Pred>;

template<class TypeList, unsigned Index>
using Get = detail::typelist::get<TypeList, Index>;

template<class... Bn>
using Conjunction = detail::conjunction<Bn...>;

template<class X, class R>
using HasValueType = decltype (detail::hasValueType<R>(detail::wrap<X>()));

template<class X, class R>
using HasSessionType = decltype (detail::hasSessionType<R>(detail::wrap<X>()));

template<class X, class R>
using HasIteratorType = decltype (detail::hasIteratorType<R>(detail::wrap<X>()));

} // namespace traits
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_TRAITS_HXX
