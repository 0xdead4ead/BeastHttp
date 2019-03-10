#if not defined BEASTHTTP_BASE_TIMER_HXX
#define BEASTHTTP_BASE_TIMER_HXX

#include <base/traits.hxx>

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

namespace _0xdead4ead {
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

    using duration_type = typename timer_type::duration;

    using time_point = typename timer_type::time_point;

    static_assert (traits::HasDuration<timer_type, void>::value
                   and traits::HasTimePoint<timer_type, void>::value
                   and traits::HasClockType<timer_type, void>::value,
                   "Invalid timer type!");

    template<class Time>
    explicit
    timer(io_context::executor_type, const Time&);

    timer(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    timer_type&
    stream();

    template<class F>
    void
    async_wait(F&&);

private:

    strand_type strand_;
    timer_type timer_;

}; // class timer

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/timer.ixx>

#endif // not defined BEASTHTTP_BASE_TIMER_HXX
