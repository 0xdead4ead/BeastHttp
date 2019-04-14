#if not defined BEASTHTTP_REACTOR_SESSION_HXX
#define BEASTHTTP_REACTOR_SESSION_HXX

#include <base/cb.hxx>
#include <base/request_processor.hxx>
#include <base/queue.hxx>
#include <base/timer.hxx>
#include <base/regex.hxx>
#include <base/strand_stream.hxx>
#include <base/lockable.hxx>

#include <shared/connection.hxx>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/string_body.hpp>

#define BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(router) \
    flesh_type(std::declval<socket_type>(), \
               router.resource_map(), \
               router.method_map(), \
               router.regex_flags(), \
               &router.mutex(), \
               std::declval<buffer_type>(), \
               std::declval<_OnAction>()...)

#define BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY() \
    flesh_type(std::declval<socket_type>(), \
           std::declval<std::shared_ptr<resource_map_type>>(), \
           std::declval<std::shared_ptr<method_map_type>>(), \
           std::declval<regex_flag_type>(), \
           std::declval<mutex_type*>(), \
           std::declval<buffer_type>(), \
           std::declval<_OnAction>()...)

namespace _0xdead4ead {
namespace http {
namespace reactor {

template</*Message's body*/
         class Body = boost::beast::http::string_body,
         /*Message's buffer*/
         class Buffer = boost::beast::flat_buffer,
         /*connection*/
         class Protocol = boost::asio::ip::tcp,
         template<typename> class Socket = boost::asio::basic_stream_socket,
         /*timer*/
         class Clock = boost::asio::chrono::steady_clock,
         template<typename, typename...> class Timer = boost::asio::basic_waitable_timer,
         /*Callback list*/
         template<typename> class Entry = std::function,
         template<typename, typename...> class Container = std::vector,
         /*Resources container*/
         template<typename Key, typename Value, typename... Args> class MethodMap = std::map,
         template<typename Key, typename Value, typename... Args> class ResourceMap = std::unordered_map,
         /*On error handler*/
         template<typename> class OnError = std::function,
         /*On timer expired handler*/
         template<typename> class OnTimer = std::function>
class session
{
    using self_type = session;

public:

    template<class>
    class context;

    class flesh;

    using flesh_type = flesh;

    using context_type = context<flesh_type>;

    using resource_regex_type = std::string;

    using resource_type = boost::beast::string_view;

    using method_type = boost::beast::http::verb;

    using mutex_type = base::lockable::mutex_type;

    using body_type = Body;

    using cbexecutor_type = base::cb::executor;

    using request_type = boost::beast::http::request<body_type>;

    template<class _Body>
    using response_type = boost::beast::http::response<_Body>;

    using queue_type = base::queue<flesh>;

    using buffer_type = Buffer;

    using connection_type = shared::connection<Protocol, Socket, base::strand_stream::asio_type>;

    using socket_type = typename connection_type::socket_type;

    using timer_type = base::timer<Clock, Timer, base::strand_stream::asio_type>;

    using duration_type = typename timer_type::duration_type;

    using time_point_type = typename timer_type::time_point;

    using clock_type = typename timer_type::clock_type;

    using regex_type = base::regex;

    using regex_flag_type = typename regex_type::flag_type;

    using on_error_type = OnError<void (boost::system::error_code, boost::string_view)>;

    using on_timer_type = OnTimer<void (context_type)>;

    using storage_type = base::cb::storage<self_type, Entry, Container>;

    using resource_map_type = ResourceMap<resource_regex_type, std::shared_ptr<storage_type>>;

    using method_map_type = MethodMap<method_type, resource_map_type>;

    using shutdown_type = typename socket_type::shutdown_type;

    static_assert (base::traits::TryInvoke<on_timer_type, void(context_type)>::value,
                   "Invalid OnTimer handler type!");

    static_assert (base::traits::TryInvoke<on_error_type, void(boost::system::error_code, boost::string_view)>::value,
                   "Invalid OnError handler type!");

    class flesh : private base::strand_stream, base::request_processor<self_type>,
            public std::enable_shared_from_this<flesh>
    {
        using base_type = base::request_processor<self_type>;

        friend queue_type;

    public:

        socket_type&
        asio_socket();

        flesh&
        recv();

        flesh&
        recv(duration_type const);

        flesh&
        recv(time_point_type const);

        template<class _OtherBody>
        flesh&
        send(response_type<_OtherBody>&);

        template<class _OtherBody>
        flesh&
        send(response_type<_OtherBody>&&);

        template<class _OtherBody>
        flesh&
        send(response_type<_OtherBody>&, duration_type const);

        template<class _OtherBody>
        flesh&
        send(response_type<_OtherBody>&&, duration_type const);

        template<class _OtherBody>
        flesh&
        send(response_type<_OtherBody>&, time_point_type const);

        template<class _OtherBody>
        flesh&
        send(response_type<_OtherBody>&&, time_point_type const);

        flesh&
        eof();

        flesh&
        cls();

        explicit
        flesh(socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
              buffer_type&&);

        template<class _OnError>
        explicit
        flesh(socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
              buffer_type&&,
              _OnError&&,
              typename std::enable_if<
              base::traits::TryInvoke<_OnError,
              void(boost::system::error_code,
                   boost::string_view)>::value, int>::type = 0);

        template<class _OnError, class _OnTimer>
        explicit
        flesh(socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
              buffer_type&&,
              _OnError&&,
              _OnTimer&&,
              typename std::enable_if<
              base::traits::TryInvoke<_OnError,
              void(boost::system::error_code,
                   boost::string_view)>::value and
              base::traits::TryInvoke<_OnTimer,
              void(context_type)>::value, int>::type = 0);

    private:

        void
        on_timer(boost::system::error_code);

        void
        on_read(boost::system::error_code, std::size_t);

        void
        on_write(boost::system::error_code, std::size_t, bool);

        template<class _OtherBody>
        void
        do_write(response_type<_OtherBody>&);

        void
        do_read();

        void
        do_eof();

        void
        do_cls();

        void
        do_launch_timer();

        void
        do_timeout();

        void
        do_process_request();

        socket_type&
        get_asio_socket();

        mutex_type* router_mutex_;

        on_error_type on_error_;
        on_timer_type on_timer_;

        timer_type timer_;

        connection_type connection_;
        request_type request_;
        buffer_type buffer_;
        queue_type queue_;

    }; // class flesh

    template<class Flesh>
    class context
    {
        Flesh& flesh_;

    public:

        context(Flesh& flesh)
            : flesh_{flesh}
        {
        }

        socket_type&
        asio_socket()
        {
            return flesh_.asio_socket();
        }

        context&
        recv() const &
        {
            flesh_.recv();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        recv(duration_type const duration) const &
        {
            flesh_.recv(duration);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        recv(time_point_type const time_point) const &
        {
            flesh_.recv(time_point);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class _OtherBody>
        context&
        send(response_type<_OtherBody>& response) const &
        {
            flesh_.send(response);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class _OtherBody>
        context&
        send(response_type<_OtherBody>&& response) const &
        {
            flesh_.send(std::move(response));
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class _OtherBody>
        context&
        send(response_type<_OtherBody>& response,
             duration_type const duration) const &
        {
            flesh_.send(response, duration);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class _OtherBody>
        context&
        send(response_type<_OtherBody>&& response,
             duration_type const duration) const &
        {
            flesh_.send(std::move(response), duration);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class _OtherBody>
        context&
        send(response_type<_OtherBody>& response,
             time_point_type const time_point) const &
        {
            flesh_.send(response, time_point);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class _OtherBody>
        context&
        send(response_type<_OtherBody>&& response,
             time_point_type const time_point) const &
        {
            flesh_.send(std::move(response), time_point);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        eof() const &
        {
            flesh_.eof();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        cls() const &
        {
            flesh_.cls();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

    }; // class context

    template<class Router, class... _OnAction>
    static auto
    recv(socket_type&&, Router const&, buffer_type&&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
            std::declval<flesh_type&>());

    template<class Router, class... _OnAction>
    static auto
    recv(socket_type&&, Router const&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
            std::declval<flesh_type&>());

    template<class Router, class TimePointOrDuration, class... _OnAction>
    static auto
    recv(socket_type&&, Router const&, TimePointOrDuration const, buffer_type&&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).recv(std::declval<TimePointOrDuration>()),
            std::declval<flesh_type&>());

    template<class Router, class TimePointOrDuration, class... _OnAction>
    static auto
    recv(socket_type&&, Router const&, TimePointOrDuration const, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).recv(std::declval<TimePointOrDuration>()),
            std::declval<flesh_type&>());

    template<class Router, class Response, class... _OnAction>
    static auto
    send(socket_type&&, Response&&, Router const&, buffer_type&&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).send(std::declval<Response>()),
            std::declval<flesh_type&>());

    template<class Router, class Response, class... _OnAction>
    static auto
    send(socket_type&&, Response&&, Router const&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).send(std::declval<Response>()),
            std::declval<flesh_type&>());

    template<class Router, class Response, class TimePointOrDuration, class... _OnAction>
    static auto
    send(socket_type&&, Response&&, Router const&, TimePointOrDuration const, buffer_type&&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).send(std::declval<Response>(), std::declval<TimePointOrDuration>()),
            std::declval<flesh_type&>());

    template<class Router, class Response, class TimePointOrDuration, class... _OnAction>
    static auto
    send(socket_type&&, Response&&, Router const&, TimePointOrDuration const, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).send(std::declval<Response>(), std::declval<TimePointOrDuration>()),
            std::declval<flesh_type&>());

    template<class... _OnAction>
    static auto
    eof(socket_type&&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY(),
            std::declval<flesh_type&>());

    template<class... _OnAction>
    static auto
    cls(socket_type&&, _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY(),
            std::declval<flesh_type&>());

}; // class session

namespace _default {

using session_type = session<>;

} // namespace _default
} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#include <reactor/impl/session.hxx>

#endif // not defined BEASTHTTP_REACTOR_SESSION_HXX
