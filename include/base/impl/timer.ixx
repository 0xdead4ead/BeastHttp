#if not defined BEASTHTTP_BASE_TIMER_IXX
#define BEASTHTTP_BASE_TIMER_IXX

#include <boost/asio/bind_executor.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Clock,
         template<typename, typename...> class Timer>
template<class Time>
timer<Clock, Timer>::timer(io_context::executor_type executor,
                           const Time& duration_or_time)
    : strand_{executor},
      timer_{executor.context(), duration_or_time}
{
}

template<class Clock,
         template<typename, typename...> class Timer>
typename timer<Clock, Timer>::timer_type&
timer<Clock, Timer>::stream()
{
    return timer_;
}

template<class Clock,
         template<typename, typename...> class Timer>
template<class F>
void
timer<Clock, Timer>::async_wait(F&& f)
{
    timer_.async_wait(
                boost::asio::bind_executor(
                    strand_, std::forward<F>(f)));
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_TIMER_IXX
