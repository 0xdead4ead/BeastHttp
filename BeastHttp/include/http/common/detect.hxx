#if not defined BEASTHTTP_COMMON_DETECT_HXX
#define BEASTHTTP_COMMON_DETECT_HXX

#include <http/base/traits.hxx>
#include <http/base/timer.hxx>
#include <http/base/detect.hxx>
#include <http/base/strand_stream.hxx>

#include <boost/asio/ip/tcp.hpp>

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/string_param.hpp>

#define BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES \
    Buffer, Protocol, Socket, Clock, Timer, OnError, OnDetect, OnTimer

namespace _0xdead4ead {
namespace http {
namespace common {

template</*Message's buffer*/
         class Buffer = boost::beast::flat_buffer,
         /*connection*/
         class Protocol = boost::asio::ip::tcp,
         class Socket = boost::asio::basic_stream_socket<Protocol>,
         /*timer*/
         class Clock = boost::asio::chrono::steady_clock,
         class Timer = boost::asio::basic_waitable_timer<Clock>,
         /*On error/warning handler holder*/
         template<typename> class OnError = std::function,
         /*On detect tls hello packet handler holder*/
         template<typename> class OnDetect = std::function,
         /*On timer expired handler holder*/
         template<typename> class OnTimer = std::function>
class detect : public std::enable_shared_from_this<detect<BEASTHTTP_COMMON_DETECT_TMPL_ATTRIBUTES>>,
        private base::strand_stream, base::detect<base::strand_stream::asio_type>, boost::asio::coroutine
{
public:

    using self_type = detect;

    using base_type = base::detect<base::strand_stream::asio_type>;

    using protocol_type = Protocol;

    using socket_type = Socket;

    using buffer_type = Buffer;

    using timer_type = base::timer<Timer, base::strand_stream::asio_type>;

    using duration_type = typename timer_type::duration_type;

    using time_point_type = typename timer_type::time_point;

    using clock_type = typename timer_type::clock_type;

    using on_detect_type = OnDetect<void (socket_type, buffer_type, boost::tribool)>;

    using on_timer_type = OnTimer<void (std::reference_wrapper<socket_type>)>;

    using on_error_type = OnError<void (boost::system::error_code, boost::string_view)>;

    template<class... _OnAction>
    static auto
    async(socket_type&& socket, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)));

    template<class... _OnAction>
    static auto
    async(socket_type&& socket, duration_type const duration, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)));

    template<class... _OnAction>
    static auto
    async(socket_type&& socket, time_point_type const time_point, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)));


    template<class Deleter, class Allocator, class... _OnAction>
    static auto
    async(std::allocator_arg_t, socket_type&& socket, const Deleter& d,
          const Allocator& alloc, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)));

    template<class Deleter, class Allocator, class... _OnAction>
    static auto
    async(std::allocator_arg_t, socket_type&& socket, duration_type const duration,
          const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)));

    template<class Deleter, class Allocator, class... _OnAction>
    static auto
    async(std::allocator_arg_t, socket_type&& socket, time_point_type const time_point,
          const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> decltype (
            void(self_type(std::declval<socket_type>(), std::declval<_OnAction>()...)));

    static boost::system::error_code
    sync(socket_type& socket, buffer_type& buffer, boost::tribool& result);

    template<class _OnDetect>
    explicit
    detect(socket_type&& socket, _OnDetect&& on_detect,
             typename std::enable_if<base::traits::TryInvoke<
             _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value, int>::type = 0);

    template<class _OnDetect, class _OnError>
    explicit
    detect(socket_type&& socket, _OnDetect&& on_detect, _OnError&& on_error,
             typename std::enable_if<base::traits::TryInvoke<
             _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value and
             base::traits::TryInvoke<_OnError, void(
                 boost::system::error_code, boost::string_view)>::value, int>::type = 0);

    template<class _OnDetect, class _OnError, class _OnTimer>
    explicit
    detect(socket_type&& socket, _OnDetect&& on_detect, _OnError&& on_error, _OnTimer&& on_timer,
             typename std::enable_if<base::traits::TryInvoke<
             _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value and
             base::traits::TryInvoke<_OnError, void(
                 boost::system::error_code, boost::string_view)>::value and
             base::traits::TryInvoke<_OnTimer, void(
                 std::reference_wrapper<socket_type>)>::value, int>::type = 0);

private:

    void
    do_async(boost::system::error_code = {}, boost::tribool = {});

    void
    do_async_2(duration_type, boost::system::error_code = {}, boost::tribool = {});

    void
    do_async_2(time_point_type, boost::system::error_code = {}, boost::tribool = {});

    void
    do_launch_timer();

    void
    on_timer(boost::system::error_code);

    void
    do_timer_cancel();

    socket_type socket_;
    buffer_type buffer_;
    timer_type timer_;

    on_detect_type on_detect_;
    on_timer_type on_timer_;
    on_error_type on_error_;

};

namespace _default {

using detect_type = common::detect<>;

} // namespace _default
} // namespace common
} // namespace http
} // namespace _0xdead4ead

#include <http/common/impl/detect.hxx>

#endif // not defined BEASTHTTP_COMMON_DETECT_HXX
