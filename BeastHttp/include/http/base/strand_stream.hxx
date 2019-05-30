#if not defined BEASTHTTP_BASE_STRAND_STREAM_HXX
#define BEASTHTTP_BASE_STRAND_STREAM_HXX

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

struct strand_stream :
        boost::asio::strand<boost::asio::system_timer::executor_type>
{
    using asio_type = boost::asio::strand<boost::asio::system_timer::executor_type>;

    strand_stream(const boost::asio::system_timer::executor_type& executor)
        : asio_type(executor)
    {
    }
};

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_STRAND_STREAM_HXX
