#if not defined BEASTHTTP_COMMON_IMPL_DETECTOR_HXX
#define BEASTHTTP_COMMON_IMPL_DETECTOR_HXX

#define BEASTHTTP_COMMON_DETECTOR_TMPL_DECLARE \
    template<template<typename> class OnDetect, \
             template<typename> class OnError, \
             class Buffer, \
             class Protocol, \
             template<typename> class Socket>

namespace _0xdead4ead {
namespace http {
namespace common {

BEASTHTTP_COMMON_DETECTOR_TMPL_DECLARE
template<class... _OnAction>
auto
detector<BEASTHTTP_COMMON_DETECTOR_TMPL_ATTRIBUTES>::async(
        socket_type socket, _OnAction&&... on_action)  -> decltype (
        self_type(std::declval<socket_type>(), std::declval<_OnAction>()...), void())
{
    std::make_shared<self_type>(std::move(socket), std::forward<_OnAction>(on_action)...)->do_async();
}

BEASTHTTP_COMMON_DETECTOR_TMPL_DECLARE
boost::system::error_code
detector<BEASTHTTP_COMMON_DETECTOR_TMPL_ATTRIBUTES>::sync(
        socket_type& socket, buffer_type& buffer, boost::tribool& result)
{
    return base_type::sync(socket, buffer, result);
}

BEASTHTTP_COMMON_DETECTOR_TMPL_DECLARE
template<class _OnDetect>
detector<BEASTHTTP_COMMON_DETECTOR_TMPL_ATTRIBUTES>::detector(
        socket_type socket, _OnDetect&& on_detect,
        typename std::enable_if<base::traits::TryInvoke<
        _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value, int>::type)
    : base::strand_stream(socket.get_executor()),
      base_type(static_cast<base::strand_stream&>(*this)),
      socket_(std::move(socket)),
      on_detect_{std::forward<_OnDetect>(on_detect)}
{
}

BEASTHTTP_COMMON_DETECTOR_TMPL_DECLARE
template<class _OnDetect, class _OnError>
detector<BEASTHTTP_COMMON_DETECTOR_TMPL_ATTRIBUTES>::detector(
        socket_type socket, _OnDetect&& on_detect, _OnError&& on_error,
        typename std::enable_if<base::traits::TryInvoke<
        _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value and
        base::traits::TryInvoke<_OnError, void(
            boost::system::error_code, boost::string_view)>::value, int>::type)
    : base::strand_stream(socket.get_executor()),
      base_type(static_cast<base::strand_stream&>(*this)),
      socket_(std::move(socket)),
      on_detect_{std::forward<_OnDetect>(on_detect)},
      on_error_{std::forward<_OnError>(on_error)}
{
}

BEASTHTTP_COMMON_DETECTOR_TMPL_DECLARE
void
detector<BEASTHTTP_COMMON_DETECTOR_TMPL_ATTRIBUTES>::do_async(
        boost::system::error_code ec, boost::tribool result)
{
    BOOST_ASIO_CORO_REENTER(*this) {
        BOOST_ASIO_CORO_YIELD
                base_type::async(socket_, buffer_,
                                 std::bind(&self_type::do_async,
                                           this->shared_from_this(),
                                           std::placeholders::_1,
                                           std::placeholders::_2));
        if(ec) {
            on_error_(ec, "detect/do_async");
            return;
        }

        on_detect_(std::move(socket_), std::move(buffer_), result);
    }
}

} // namespace common
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_COMMON_IMPL_DETECTOR_HXX
