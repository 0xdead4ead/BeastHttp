#if not defined BEASTHTTP_BASE_DETECTOR_HXX
#define BEASTHTTP_BASE_DETECTOR_HXX

#include <boost/beast/core/error.hpp>
#include <boost/logic/tribool.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class CompletionExecutor>
class detector
{
protected:

    CompletionExecutor const& completion_executor_;

    explicit
    detector(CompletionExecutor const&);

    template<class S, class B, class F>
    void
    async(S&, B&, F&&);

    template<class S, class B>
    static boost::beast::error_code
    sync(S&, B&, boost::tribool&);

};

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/detector.hxx>

#endif // not defined BEASTHTTP_BASE_DETECTOR_HXX
