#if not defined BEASTHTTP_BASE_TIMER_HXX
#define BEASTHTTP_BASE_TIMER_HXX

#include <http/base/traits.hxx>

#include <boost/system/error_code.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

using namespace traits;

template<class T,
         class CompletionExecutor>
class timer
{
    using self_type = timer;

public:

    using timer_type = T;

    using clock_type = typename timer_type::clock_type;

    using duration_type = typename timer_type::duration;

    using time_point = typename timer_type::time_point;

    static_assert (traits::Conjunction<
                   traits::HasDuration<timer_type, void>,
                   traits::HasTimePoint<timer_type, void>,
                   traits::HasClockType<timer_type, void>,
                   traits::TryWait<timer_type, void(boost::system::error_code&)>,
                   traits::TryCancel<timer_type, void(boost::system::error_code&)>,
                   traits::TryAsyncWait<timer_type, void(void(boost::system::error_code))>>::value,
                   "Invalid timer type!");

    template<class TimePointOrDuration>
    explicit
    timer(const CompletionExecutor&, const TimePointOrDuration);

    timer_type&
    stream();

    template<class F>
    void
    async_wait(F&&);

    boost::system::error_code
    wait();

    boost::system::error_code
    cancel();

private:

    CompletionExecutor const& completion_executor_;
    timer_type timer_;

}; // class timer

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <http/base/impl/timer.hxx>

#endif // not defined BEASTHTTP_BASE_TIMER_HXX
