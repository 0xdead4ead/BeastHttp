#if not defined BEASTHTTP_BASE_DISPLAY_HXX
#define BEASTHTTP_BASE_DISPLAY_HXX

#include <http/base/config.hxx>

#include <http/base/traits.hxx>

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

#if not defined BEASTHTTP_CXX17_FOLD_EXPR
template<class Stream, class Arg, class... Args>
auto
print(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, void(Arg)>::value, int>::type{}, stream.flush());

template<class Stream, class Arg, class... Args>
auto
printline(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, void(Arg)>::value, int>::type{}, stream.flush());
#else
template<class Stream, class... Args>
auto
print(Stream& stream, Args const&... args)
-> decltype (std::enable_if_t<(... and traits::TryLeftShift<Stream, void(Args)>::value), int>{}, stream.flush());

template<class Stream, class... Args>
auto
printline(Stream& stream, Args const&... args)
-> decltype (std::enable_if_t<(... and traits::TryLeftShift<Stream, void(Args)>::value), int>{}, stream.flush());
#endif // BEASTHTTP_CXX17_FOLD_EXPR

template<const char s, class Stream, class Arg, class... Args>
auto
print(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, void(Arg)>::value, int>::type{}, stream.flush());

template<const char s, class Stream, class Arg, class... Args>
auto
printline(Stream& stream, Arg const& arg, Args const&... args)
-> decltype (typename std::enable_if<
             traits::TryLeftShift<Stream, void(Arg)>::value, int>::type{}, stream.flush());

} // namespace display
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <http/base/impl/display.hxx>

#endif // not defined BEASTHTTP_BASE_DISPLAY_HXX
