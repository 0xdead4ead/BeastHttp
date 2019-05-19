#if not defined BEASTHTTP_BASE_DETECT_HXX
#define BEASTHTTP_BASE_DETECT_HXX

#include <boost/beast/core/error.hpp>
#include <boost/logic/tribool.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class CompletionExecutor>
class detect
{
protected:

    CompletionExecutor const& completion_executor_;

    explicit
    detect(CompletionExecutor const&);

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

#include <http/base/impl/detect.hxx>

#endif // not defined BEASTHTTP_BASE_DETECT_HXX
