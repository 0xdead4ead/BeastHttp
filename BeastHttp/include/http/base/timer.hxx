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
