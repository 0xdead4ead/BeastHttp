#if not defined BEASTHTTP_BASE_IMPL_TIMER_HXX
#define BEASTHTTP_BASE_IMPL_TIMER_HXX

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/version.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Clock,
         template<typename, typename...> class Timer, class CompletionExecutor>
template<class TimePointOrDuration>
timer<Clock, Timer, CompletionExecutor>::timer(
        const CompletionExecutor& completion_executor,
        const TimePointOrDuration duration_or_time)
    : completion_executor_{completion_executor},
      timer_{completion_executor.get_inner_executor()
#if BOOST_ASIO_VERSION < 101400
        .context()
#endif
      , duration_or_time}
{
}

template<class Clock,
         template<typename, typename...> class Timer, class CompletionExecutor>
typename timer<Clock, Timer, CompletionExecutor>::timer_type&
timer<Clock, Timer, CompletionExecutor>::stream()
{
    return timer_;
}

template<class Clock,
         template<typename, typename...> class Timer,
         class CompletionExecutor>
template<class F>
void
timer<Clock, Timer, CompletionExecutor>::async_wait(F&& f)
{
    timer_.async_wait(
                boost::asio::bind_executor(
                    completion_executor_, std::forward<F>(f)));
}

template<class Clock,
         template<typename, typename...> class Timer, class CompletionExecutor>
boost::system::error_code
timer<Clock, Timer, CompletionExecutor>::wait()
{
    auto ec = boost::system::error_code{};

    timer_.wait(ec);

    return ec;
}

template<class Clock,
         template<typename, typename...> class Timer, class CompletionExecutor>
boost::system::error_code
timer<Clock, Timer, CompletionExecutor>::cancel()
{
    auto ec = boost::system::error_code{};

    timer_.cancel(ec);

    return ec;
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_TIMER_HXX
