#if not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
#define BEASTHTTP_REACTOR_SSL_SESSION_HXX

#include <base/cb.hxx>
#include <base/request_processor.hxx>
#include <base/queue.hxx>
#include <base/timer.hxx>
#include <base/regex.hxx>

#include <shared/ssl/connection.hxx>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/string_body.hpp>

namespace _0xdead4ead {
namespace http {
namespace reactor {
namespace ssl {

/**
  @brief session class. Handles an HTTP server connection
  @tparam Type of body request message
*/
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

    using reference_wrapper = std::reference_wrapper<context_type const>;

    using resource_regex_type = std::string;

    using resource_type = boost::beast::string_view;

    using method_type = boost::beast::http::verb;

    using body_type = Body;

    using cbexecutor_type = base::cb::executor;

    using request_type = boost::beast::http::request<body_type>;

    using queue_type = base::queue<flesh>;

    using buffer_type = Buffer;

    using connection_type = shared::ssl::connection<Protocol, Socket>;

    using socket_type = typename connection_type::socket_type;

    using timer_type = base::timer<Clock, Timer>;

    using duration_type = typename timer_type::duration_type;

    using time_point_type = typename timer_type::time_point;

    using clock_type = typename timer_type::clock_type;

    using regex_type = base::regex;

    using regex_flag_type = typename regex_type::flag_type;

    using on_error_type = OnError<void (boost::system::error_code, const char*)>;

    using on_handshake_type = OnHandshake<void (reference_wrapper)>;

    using on_timer_type = OnTimer<void (reference_wrapper)>;

    using storage_type = base::cb::storage<self_type, Entry, Container>;

    using resource_map_type = ResourceMap<resource_regex_type, std::shared_ptr<storage_type>>;

    using method_map_type = MethodMap<method_type, resource_map_type>;

    using shutdown_type = typename socket_type::shutdown_type;

    static_assert (base::traits::TryInvoke<on_timer_type, reference_wrapper>::value,
                   "Invalid OnTimer handler!");

    static_assert (base::traits::TryInvoke<on_handshake_type, reference_wrapper>::value,
                   "Invalid OnHandshake handler!");

    static_assert (base::traits::TryInvoke<on_error_type, boost::system::error_code, const char*>::value,
                   "Invalid OnError handler!");

    class flesh : private base::request_processor<self_type>,
            public std::enable_shared_from_this<flesh>
    {
        using base_type = base::request_processor<self_type>;

        friend queue_type;

    public:

        connection_type&
        getConnection();

        flesh&
        handshake();

        flesh&
        handshake(duration_type const&);

        flesh&
        handshake(time_point_type const&);

        flesh&
        recv();

        flesh&
        recv(duration_type const&);

        flesh&
        recv(time_point_type const&);

        template<class Response>
        flesh&
        send(Response&&);

        template<class Response>
        flesh&
        send(Response&&, duration_type const&);

        template<class Response>
        flesh&
        send(Response&&, time_point_type const&);

        flesh&
        eof();

        flesh&
        eof(duration_type const&);

        flesh&
        eof(time_point_type const&);

        flesh&
        force_eof();

        flesh&
        launch_timer();

        template<class _OnHandshake>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnHandshake&&);

        template<class _OnHandshake, class _OnError>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnHandshake&&,
              _OnError&&);

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
              _OnTimer&&);

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

        template<class Response>
        void
        do_write(Response&);

        void
        do_read();

        void
        do_eof();

        void
        do_force_eof();

        void
        do_timeout();

        void
        process_request();

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

        connection_type&
        getConnection() const &
        {
            return flesh_.getConnection();
        }

        context&
        recv() const &
        {
            flesh_.recv();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class TimeOrDuration>
        context&
        recv(TimeOrDuration const& time_or_duration) const &
        {
            flesh_.recv(time_or_duration);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class Response>
        context&
        send(Response&& response) const &
        {
            flesh_.send(std::forward<Response>(response));
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        template<class Response, class TimeOrDuration>
        context&
        send(Response&& response,
             TimeOrDuration const& time_or_duration) const &
        {
            flesh_.send(std::forward<Response>(response), time_or_duration);
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

        template<class TimeOrDuration>
        context&
        eof(TimeOrDuration const& time_or_duration) const &
        {
            flesh_.eof(time_or_duration);
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

        context&
        launch_timer() const &
        {
            flesh_.launch_timer();
            return const_cast<typename std::add_lvalue_reference<
                    context>::type>(*this);
        }

    }; // class context

    template<class... _OnAction>
    static flesh_type&
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnAction&&...);

    template<class... _OnAction>
    static flesh_type&
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...);

    template<class... _OnAction>
    static flesh_type&
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              duration_type const&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnAction&&...);

    template<class... _OnAction>
    static flesh_type&
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              duration_type const&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...);

    template<class... _OnAction>
    static flesh_type&
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              time_point_type const&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              buffer_type&&,
              _OnAction&&...);

    template<class... _OnAction>
    static flesh_type&
    handshake(boost::asio::ssl::context&,
              socket_type&&,
              time_point_type const&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              _OnAction&&...);

    template<class... _OnAction>
    static flesh_type&
    eof(boost::asio::ssl::context&,
        socket_type&&,
        std::shared_ptr<resource_map_type> const&,
        std::shared_ptr<method_map_type> const&,
        regex_flag_type,
        _OnAction&&...);

}; // class session

namespace _default {

using session_type = session<>;

} // namespace _default
} // namespace ssl
} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#include <reactor/ssl/impl/session.ixx>

#endif // not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
