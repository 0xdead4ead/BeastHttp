#if not defined BEASTHTTP_BASE_DETECTOR_HXX
#define BEASTHTTP_BASE_DETECTOR_HXX

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/logic/tribool.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

class detector
{

protected:

    using io_context = boost::asio::io_context;

    using strand_type = boost::asio::strand<io_context::executor_type>;

    strand_type strand_;

    explicit
    detector(io_context::executor_type);

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
