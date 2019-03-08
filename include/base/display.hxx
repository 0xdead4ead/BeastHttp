#if not defined BEASTHTTP_BASE_DISPLAY_HXX
#define BEASTHTTP_BASE_DISPLAY_HXX

#include <base/traits.hxx>

namespace _0xdead4ead {
namespace http {
namespace base {
namespace display {

template<class Stream>
auto
print(Stream& stream) -> decltype (stream.flush());

template<const char, class Stream>
auto
print(Stream& stream) -> decltype (stream.flush());

template<class Stream>
auto
printline(Stream& stream) -> decltype (stream.flush());

template<const char, class Stream>
auto
printline(Stream& stream) -> decltype (stream.flush());

#if not defined __cpp_fold_expressions
template<class Stream, class Arg, class... Args>
auto
print(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, Arg>::value, int>::type{}, stream.flush());

template<class Stream, class Arg, class... Args>
auto
printline(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, Arg>::value, int>::type{}, stream.flush());
#else
template<class Stream, class... Args>
auto
print(Stream& stream, Args const&... args)
-> decltype (std::enable_if_t<(... and traits::TryLeftShift<Stream, Args>::value), int>{}, stream.flush());

template<class Stream, class... Args>
auto
printline(Stream& stream, Args const&... args)
-> decltype (std::enable_if_t<(... and traits::TryLeftShift<Stream, Args>::value), int>{}, stream.flush());
#endif // not defined __cpp_fold_expressions

template<const char s, class Stream, class Arg, class... Args>
auto
print(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, Arg>::value, int>::type{}, stream.flush());

template<const char s, class Stream, class Arg, class... Args>
auto
printline(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, Arg>::value, int>::type{}, stream.flush());

} // namespace display
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/display.ixx>

#endif // not defined BEASTHTTP_BASE_DISPLAY_HXX
