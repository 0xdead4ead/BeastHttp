#if not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
#define BEASTHTTP_REACTOR_SSL_SESSION_HXX

#include <base/cb.hxx>
#include <base/request_processor.hxx>
#include <base/queue.hxx>
#include <base/timer.hxx>
#include <base/regex.hxx>
#include <base/strand_stream.hxx>

#include <shared/ssl/connection.hxx>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/string_body.hpp>

#define BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE() \
    flesh_type(std::declval<boost::asio::ssl::context&>(), \
               std::declval<socket_type>(), \
               std::declval<std::shared_ptr<resource_map_type>>(), \
               std::declval<std::shared_ptr<method_map_type>>(), \
               std::declval<regex_flag_type>(), \
               std::declval<buffer_type>(), \
               std::declval<_OnAction>()...)

namespace _0xdead4ead {
namespace http {
namespace reactor {
namespace ssl {

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
         template<typename> class OnTimer = std::function,
         /*On handshake handler*/
         template<typename> class OnHandshake = std::function>
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

    using body_type = Body;

    using cbexecutor_type = base::cb::executor;

    using request_type = boost::beast::http::request<body_type>;

    template<class _OtherBody>
    using response_type = boost::beast::http::response<_OtherBody>;

    using queue_type = base::queue<flesh>;

    using buffer_type = Buffer;

    using connection_type = shared::ssl::connection<Protocol, Socket, base::strand_stream::asio_type>;

    using socket_type = typename connection_type::socket_type;

    using ssl_stream_type = typename connection_type::ssl_stream_type;

    using timer_type = base::timer<Clock, Timer, base::strand_stream::asio_type>;

    using duration_type = typename timer_type::duration_type;

    using time_point_type = typename timer_type::time_point;

    using clock_type = typename timer_type::clock_type;

    using regex_type = base::regex;

    using regex_flag_type = typename regex_type::flag_type;

    using on_error_type = OnError<void (boost::system::error_code, boost::string_view)>;

    using on_handshake_type = OnHandshake<void (context_type)>;

    using on_timer_type = OnTimer<void (context_type)>;

    using storage_type = base::cb::storage<self_type, Entry, Container>;

    using resource_map_type = ResourceMap<resource_regex_type, std::shared_ptr<storage_type>>;

    using method_map_type = MethodMap<method_type, resource_map_type>;

    using shutdown_type = typename socket_type::shutdown_type;

    static_assert (base::traits::TryInvoke<on_timer_type, void(context_type)>::value,
                   "Invalid OnTimer handler type!");

    static_assert (base::traits::TryInvoke<on_handshake_type, void(context_type)>::value,
                   "Invalid OnHandshake handler type!");

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

        ssl_stream_type&
        asio_ssl_stream();

        flesh&
        handshake();

        flesh&
        handshake(duration_type const);

        flesh&
        handshake(time_point_type const);

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
        eof(duration_type const);

        flesh&
        eof(time_point_type const);

        flesh&
        force_eof();

        flesh&
        force_cls();

        template<class _OnHandshake>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnHandshake&&,
              typename std::enable_if<
              base::traits::TryInvoke<_OnHandshake,
              void(context_type)>::value, int>::type = 0);

        template<class _OnHandshake, class _OnError>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnHandshake&&,
              _OnError&&,
              typename std::enable_if<
              base::traits::TryInvoke<_OnHandshake,
              void(context_type)>::value and
              base::traits::TryInvoke<_OnError,
              void(boost::system::error_code,
                   boost::string_view)>::value, int>::type = 0);

        template<class _OnHandshake, class _OnError, class _OnTimer>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnHandshake&&,
              _OnError&&,
              _OnTimer&&,
              typename std::enable_if<
              base::traits::TryInvoke<_OnHandshake,
              void(context_type)>::value and
              base::traits::TryInvoke<_OnError,
              void(boost::system::error_code,
                   boost::string_view)>::value and
              base::traits::TryInvoke<_OnTimer,
              void(context_type)>::value, int>::type = 0);

    private:

        void
        on_timer(boost::system::error_code);

        void
        on_handshake(boost::system::error_code, std::size_t);

        void
        on_shutdown(boost::system::error_code);

        void
        on_read(boost::system::error_code, std::size_t);

        void
        on_write(boost::system::error_code, std::size_t, bool);

        void
        do_handshake();

        void
        do_shutdown();

        template<class _OtherBody>
        void
        do_write(response_type<_OtherBody>&);

        void
        do_read();

        void
        do_eof();

        void
        do_force_eof();

        void
        do_force_cls();

        void
        do_launch_timer();

        void
        do_timeout();

        void
        do_process_request();

        socket_type&
        get_asio_socket();

        ssl_stream_type&
        get_asio_ssl_stream();

        bool eof_ = false;

        on_handshake_type on_handshake_;
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
        {}

        socket_type&
        asio_socket()
        {
            return flesh_.asio_socket();
        }

        ssl_stream_type&
        asio_ssl_stream()
        {
            return flesh_.asio_ssl_stream();
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
        eof(duration_type const duration) const &
        {
            flesh_.eof(duration);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        eof(time_point_type const time_point) const &
        {
            flesh_.eof(time_point);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        force_eof() const &
        {
            flesh_.force_eof();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        force_cls() const &
        {
            flesh_.force_cls();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

    }; // class context

    template<class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
            std::declval<flesh_type&>());

    template<class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
            std::declval<flesh_type&>());

    template<class TimePointOrDuration, class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              TimePointOrDuration const,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE().handshake(std::declval<TimePointOrDuration>()),
            std::declval<flesh_type&>());

    template<class TimePointOrDuration, class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              TimePointOrDuration const,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE().handshake(std::declval<TimePointOrDuration>()),
            std::declval<flesh_type&>());

    template<class... _OnAction>
    static auto
    force_eof(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
            std::declval<flesh_type&>());

    template<class... _OnAction>
    static auto
    force_cls(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...) -> decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
            std::declval<flesh_type&>());

}; // class session

namespace _default {

using session_type = session<>;

} // namespace _default
} // namespace ssl
} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#include <reactor/ssl/impl/session.hxx>

#endif // not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
