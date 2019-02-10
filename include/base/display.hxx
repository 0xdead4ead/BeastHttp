#if not defined BEASTHTTP_BASE_DISPLAY_HXX
#define BEASTHTTP_BASE_DISPLAY_HXX

#include "traits.hxx"

namespace http {
namespace base {
namespace display {

template<class Stream>
auto print(Stream& stream) -> decltype (stream.flush())
{
    return stream.flush();
}

template<const char, class Stream>
auto print(Stream& stream) -> decltype (stream.flush())
{
    return stream.flush();
}

template<class Stream>
auto printline(Stream& stream) -> decltype (stream.flush())
{
    return stream.flush();
}

template<const char, class Stream>
auto printline(Stream& stream) -> decltype (stream.flush())
{
    return stream.flush();
}

#if not defined __cpp_fold_expressions
template<class Stream, class Arg, class... Args>
auto print(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (std::enable_if_t<traits::TryLeftShift<Stream, Arg>::value, int>{}, stream.flush())
{
    stream << arg;
    if (sizeof... (args) > 0) {
        return print(stream, args...);
    }

    return stream.flush();
}

template<class Stream, class Arg, class... Args>
auto printline(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (std::enable_if_t<traits::TryLeftShift<Stream, Arg>::value, int>{}, stream.flush())
{
    stream << arg;
    if (sizeof... (args) > 0) {
        return printline(stream, args...);
    }

    stream << '\n';
    return stream.flush();
}
#else
template<class Stream, class... Args>
auto print(Stream& stream, Args const&... args)
-> decltype (std::enable_if_t<(... and traits::TryLeftShift<Stream, Args>::value), int>{}, stream.flush())
{
    (stream << ... << args);
    return stream.flush();
}

template<class Stream, class... Args>
auto printline(Stream& stream, Args const&... args)
-> decltype (std::enable_if_t<(... and traits::TryLeftShift<Stream, Args>::value), int>{}, stream.flush())
{
    (stream << ... << args) << '\n';
    return stream.flush();
}
#endif // not defined __cpp_fold_expressions

template<const char s, class Stream, class Arg, class... Args>
auto print(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (std::enable_if_t<traits::TryLeftShift<Stream, Arg>::value, int>{}, stream.flush())
{
    stream << arg;
#if not defined __cpp_if_constexpr
    if (sizeof... (args) > 0) {
#else
    if constexpr(sizeof... (args) > 0) {
#endif // not defined __cpp_if_constexpr
        stream << s;
        return print<s>(stream, args...);
    }

    return stream.flush();
}

template<const char s, class Stream, class Arg, class... Args>
auto printline(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (std::enable_if_t<traits::TryLeftShift<Stream, Arg>::value, int>{}, stream.flush())
{
    stream << arg;
#if not defined __cpp_if_constexpr
    if (sizeof... (args) > 0) {
#else
    if constexpr(sizeof... (args) > 0) {
#endif // not defined __cpp_if_constexpr
        stream << s;
        return printline<s>(stream, args...);
    }

    stream << '\n';
    return stream.flush();
}

} // namespace display
} // namespace base
} // namespace http

#endif // not defined BEASTHTTP_BASE_DISPLAY_HXX
