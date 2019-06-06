#if not defined BEASTHTTP_COMMON_IMPL_DETECT_HXX
#define BEASTHTTP_COMMON_IMPL_DETECT_HXX

#define BEASTHTTP_COMMON_DETECT_TMPL_DECLARE \
    template<class Buffer, \
             class Protocol, \
             class Socket, \
             class Clock, \
             template<typename, typename...> class Timer, \
             template<typename> class OnError, \
             template<typename> class OnDetect, \
             template<typename> class OnTimer>

namespace _0xdead4ead {
namespace http {
namespace common {

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
template<class... _OnAction>
auto
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::async(
        socket_type socket, _OnAction&&... on_action)  -> decltype (
        void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)))
{
    std::make_shared<self_type>(std::move(socket), std::forward<_OnAction>(on_action)...)->do_async();
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
template<class... _OnAction>
auto
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::async(
        socket_type socket, duration_type const duration, _OnAction&&... on_action)  -> decltype (
        void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)))
{
    std::make_shared<self_type>(std::move(socket), std::forward<_OnAction>(on_action)...)->do_async_2(duration);
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
template<class... _OnAction>
auto
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::async(
        socket_type socket, time_point_type const time_point, _OnAction&&... on_action)  -> decltype (
        void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)))
{
    std::make_shared<self_type>(std::move(socket), std::forward<_OnAction>(on_action)...)->do_async_2(time_point);
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
boost::system::error_code
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::sync(
        socket_type& socket, buffer_type& buffer, boost::tribool& result)
{
    return base_type::sync(socket, buffer, result);
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
template<class _OnDetect>
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::detect(
        socket_type socket, _OnDetect&& on_detect,
        typename std::enable_if<base::traits::TryInvoke<
        _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value, int>::type)
    : base::strand_stream(socket.get_executor()),
      base_type(static_cast<base::strand_stream&>(*this)),
      socket_(std::move(socket)),
      timer_{static_cast<base::strand_stream&>(*this), (time_point_type::max)()},
      on_detect_{std::forward<_OnDetect>(on_detect)}
{
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
template<class _OnDetect, class _OnError>
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::detect(
        socket_type socket, _OnDetect&& on_detect, _OnError&& on_error,
        typename std::enable_if<base::traits::TryInvoke<
        _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value and
        base::traits::TryInvoke<_OnError, void(
            boost::system::error_code, boost::string_view)>::value, int>::type)
    : base::strand_stream(socket.get_executor()),
      base_type(static_cast<base::strand_stream&>(*this)),
      socket_(std::move(socket)),
      timer_{static_cast<base::strand_stream&>(*this), (time_point_type::max)()},
      on_detect_{std::forward<_OnDetect>(on_detect)},
      on_error_{std::forward<_OnError>(on_error)}
{
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
template<class _OnDetect, class _OnError, class _OnTimer>
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::detect(
        socket_type socket, _OnDetect&& on_detect, _OnError&& on_error, _OnTimer&& on_timer,
        typename std::enable_if<base::traits::TryInvoke<
        _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value and
        base::traits::TryInvoke<_OnError, void(
            boost::system::error_code, boost::string_view)>::value and
        base::traits::TryInvoke<_OnTimer, void(
            std::reference_wrapper<socket_type>)>::value, int>::type)
    : base::strand_stream(socket.get_executor()),
      base_type(static_cast<base::strand_stream&>(*this)),
      socket_(std::move(socket)),
      timer_{static_cast<base::strand_stream&>(*this), (time_point_type::max)()},
      on_detect_{std::forward<_OnDetect>(on_detect)},
      on_timer_{std::forward<_OnTimer>(on_timer)},
      on_error_{std::forward<_OnError>(on_error)}
{
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
void
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::do_async(
        boost::system::error_code ec, boost::tribool result)
{
    BOOST_ASIO_CORO_REENTER(*this) {
        BOOST_ASIO_CORO_YIELD
                base_type::async(socket_, buffer_,
                                 std::bind(&self_type::do_async,
                                           this->shared_from_this(),
                                           std::placeholders::_1,
                                           std::placeholders::_2));
        do_timer_cancel();

        if (ec) {
            if (on_error_)
                on_error_(ec, "detect/do_async");

            return;
        }

        on_detect_(std::move(socket_), std::move(buffer_), result);
    }
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
void
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::do_async_2(
        duration_type duration, boost::system::error_code ec, boost::tribool result)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    do_async();
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
void
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::do_async_2(
        time_point_type time_point, boost::system::error_code ec, boost::tribool result)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    do_async();
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
void
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::do_launch_timer()
{
    timer_.async_wait(
                std::bind(
                    &self_type::on_timer,
                    this->shared_from_this(),
                    std::placeholders::_1));
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
void
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::on_timer(
        boost::system::error_code ec)
{
    if (ec and ec != boost::asio::error::operation_aborted) {
        if (on_error_)
            on_error_(ec, "async_wait/on_timer");

        return;
    }

    if (timer_.stream().expiry() <= clock_type::now()
            and on_timer_)
        on_timer_(std::ref(socket_));
}

BEASTHTTP_COMMON_DETECT_TMPL_DECLARE
void
detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>::do_timer_cancel()
{
    auto ec = timer_.cancel();

    if (ec and on_error_)
        on_error_(ec, "cancel/do_timer_cancel");
}

} // namespace common
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_COMMON_IMPL_DETECT_HXX
