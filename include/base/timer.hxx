#if not defined BEASTHTTP_BASE_TIMER_HXX
#define BEASTHTTP_BASE_TIMER_HXX

#include "traits.hxx"

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/bind_executor.hpp>

namespace http {
namespace base {

template<class Clock,
         template<typename, typename...> class Timer>
class timer
{
    using self_type = timer;

    using io_context = boost::asio::io_context;

    using strand_type = boost::asio::strand<io_context::executor_type>;

public:

    using timer_type = Timer<Clock>;

    using clock_type = typename timer_type::clock_type;

    using duration = typename timer_type::duration;

    using time_point = typename timer_type::time_point;

    static_assert (traits::HasDuration<timer_type>::value
                   and traits::HasTimePoint<timer_type>::value
                   and traits::HasClockType<timer_type>::value,
                   "Invalid timer type!");

    template<class Time>
    explicit
    timer(io_context::executor_type executor,
          const Time& duration_or_time)
        : strand_{executor},
          timer_{executor.context(), duration_or_time}
    {}

    timer(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    ~timer() = default;

    timer_type&
    stream(){
        return timer_;
    }

    template<class F>
    auto
    async_wait(F&& f){
        return timer_.async_wait(
                    boost::asio::bind_executor(
                        strand_, std::forward<F>(f)));
    }

private:

    strand_type strand_;
    timer_type timer_;

}; // class timer

} // namespace base
} // namespace http

#endif // not defined BEASTHTTP_BASE_TIMER_HXX
