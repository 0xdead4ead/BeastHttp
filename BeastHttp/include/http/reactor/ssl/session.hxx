#if not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
#define BEASTHTTP_REACTOR_SSL_SESSION_HXX

#include <unordered_map>

#include <http/base/config.hxx>

#include <http/base/cb.hxx>
#include <http/base/request_processor.hxx>
#include <http/base/queue.hxx>
#include <http/base/timer.hxx>
#include <http/base/regex.hxx>
#include <http/base/strand_stream.hxx>
#include <http/base/lockable.hxx>

#include <http/common/ssl/connection.hxx>
#include <http/common/detect.hxx>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/string_body.hpp>
#include <memory>

#if defined BEASTHTTP_CXX17_OPTIONAL
#include <optional>
#else
#include <boost/optional.hpp>
#endif

#define BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(router) \
    flesh_type(std::declval<boost::asio::ssl::context&>(), \
               std::declval<socket_type>(), \
               router.resource_map(), \
               router.method_map(), \
               router.regex_flags(), \
               &router.mutex(), \
               std::declval<buffer_type>(), \
               std::declval<_OnAction>()...)

#define BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY() \
    flesh_type(std::declval<boost::asio::ssl::context&>(), \
               std::declval<socket_type>(), \
               std::declval<std::shared_ptr<resource_map_type>>(), \
               std::declval<std::shared_ptr<method_map_type>>(), \
               std::declval<regex_flag_type>(), \
               std::declval<mutex_type*>(), \
               std::declval<buffer_type>(), \
               std::declval<_OnError>())


#define BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(overload, ...) \
    std::declval<context<Flesh, context_policy::shared>>().flesh_p_->member(overload, ##__VA_ARGS__)

#define BEASTHTTP_REACTOR_SSL_SESSION_CONTEXT_TRY_INVOKE_ADVANCE(arg, ...) \
    std::declval<context<Flesh, context_policy::shared>>().advance(arg, ##__VA_ARGS__)

namespace _0xdead4ead {
namespace http {
namespace reactor {
namespace ssl {

template</*Prototype parser/serializer for messages*/
         class Body = boost::beast::http::string_body,
         class RequestParser = boost::beast::http::request_parser<Body>,
         class ResponseSerializer = boost::beast::http::response_serializer<Body>,
         /*Message's buffer*/
         class Buffer = boost::beast::flat_buffer,
         /*Connection param's*/
         class Protocol = boost::asio::ip::tcp,
         class Socket = boost::asio::basic_stream_socket<Protocol>,
         /*Timer param's*/
         class Clock = boost::asio::chrono::steady_clock,
         class Timer = boost::asio::basic_waitable_timer<Clock>,
         /*Callback list container*/
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

    enum class context_policy { shared, weak };

    class flesh;

    template<class, context_policy>
    class context;

public:

    struct option
    {
        struct on_error_t
        {
        };

        struct on_timer_t
        {
        };

        struct get_socket_t
        {
        };

        struct get_ssl_stream_t
        {
        };
    };

    struct allocator_t
    {
    };

    using flesh_type = flesh;

    using context_type = context<flesh_type, context_policy::shared>;

    using weak_context = context<flesh_type, context_policy::weak>;

    using resource_regex_type = std::string;

    using resource_type = boost::beast::string_view;

    using method_type = boost::beast::http::verb;

    using mutex_type = base::lockable::mutex_type;

    using body_type = Body;

    using cbexecutor_type = base::cb::executor;

    using request_parser_type = RequestParser;

    using response_serializer_type = ResponseSerializer;

    using request_type = typename request_parser_type::value_type;

    template<class _Body>
    using response_type = boost::beast::http::response<_Body>;

    using queue_type = base::queue<flesh>;

    using buffer_type = Buffer;

    using connection_type = common::ssl::connection<Socket, base::strand_stream::asio_type>;

    using socket_type = typename connection_type::socket_type;

    using ssl_stream_type = typename connection_type::ssl_stream_type;

    using timer_type = base::timer<Timer, base::strand_stream::asio_type>;

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

    static constexpr typename option::on_error_t on_error_arg{};
    static constexpr typename option::on_timer_t on_timer_arg{};
    static constexpr typename option::get_socket_t get_socket_arg{};
    static constexpr typename option::get_ssl_stream_t get_ssl_stream_arg{};
    static constexpr allocator_t allocator_arg{};

private:

    class flesh : public base::strand_stream, base::request_processor<self_type>,
            public std::enable_shared_from_this<flesh>
    {
        using base_type = base::request_processor<self_type>;

        friend queue_type;

        friend context_type;

    public:

        flesh&
        handshake();

        flesh&
        handshake(duration_type const duration);

        flesh&
        handshake(time_point_type const time_point);

        flesh&
        recv();

        flesh&
        recv(duration_type const duration);

        flesh&
        recv(time_point_type const time_point);

        template<class _Body>
        flesh&
        send(response_type<_Body>&);

        template<class _Body>
        flesh&
        send(response_type<_Body>&, duration_type const);

        template<class _Body>
        flesh&
        send(response_type<_Body>&, time_point_type const);

        template<class _Body>
        flesh&
        push(response_type<_Body>&);

        flesh&
        wait();

        flesh&
        wait(duration_type const);

        flesh&
        wait(time_point_type const);

        flesh&
        timer_cancel();

        flesh&
        eof();

        flesh&
        eof(duration_type const);

        flesh&
        eof(time_point_type const);

        void
        force_eof();

        void
        force_cls();

        template<class Handler>
        void
        member(typename option::on_error_t, Handler&,
               typename std::enable_if<
               base::traits::TryInvoke<Handler,
               void(boost::system::error_code,
                    boost::string_view)>::value, int>::type = 0);

        template<class Handler>
        void
        member(typename option::on_timer_t, Handler&,
               typename std::enable_if<
               base::traits::TryInvoke<Handler,
               void(context_type)>::value, int>::type = 0);

        template<class Handler, class Allocator>
        void
        member(typename option::on_error_t, Handler&, const Allocator&,
               typename std::enable_if<
               base::traits::TryInvoke<Handler,
               void(boost::system::error_code,
                    boost::string_view)>::value and
               std::is_constructible<on_error_type, Handler, Allocator>::value, int>::type = 0);

        template<class Handler, class Allocator>
        void
        member(typename option::on_timer_t, Handler&, const Allocator&,
               typename std::enable_if<
               base::traits::TryInvoke<Handler,
               void(context_type)>::value and
               std::is_constructible<on_timer_type, Handler, Allocator>::value, int>::type = 0);

        socket_type&
        member(typename option::get_socket_t);

        ssl_stream_type&
        member(typename option::get_ssl_stream_t);

        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
              buffer_type&&);

        explicit
        flesh(int, boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
              buffer_type&&);

        template<class _OnHandshake>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
              buffer_type&&,
              _OnHandshake&&,
              typename std::enable_if<
              base::traits::TryInvoke<_OnHandshake,
              void(context_type)>::value, int>::type = 0);

        template<class _OnError>
        explicit
        flesh(int, boost::asio::ssl::context&,
              socket_type&&,
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

        template<class _OnHandshake, class _OnError>
        explicit
        flesh(boost::asio::ssl::context&,
              socket_type&&,
              std::shared_ptr<resource_map_type> const&,
              std::shared_ptr<method_map_type> const&,
              regex_flag_type,
              mutex_type*,
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
              mutex_type*,
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

        template<class _Body>
        void
        do_write(response_type<_Body>&);

        template<class _Body>
        void
        do_push(response_type<_Body>&);

        void
        do_read();

        void
        do_eof();

        void
        do_force_eof(shutdown_type);

        void
        do_force_cls();

        void
        do_launch_timer();

        void
        do_timeout();

        void
        do_process_request();

        void
        do_timer_cancel();

        bool eof_ = false;

        mutex_type* router_mutex_;

        timer_type timer_;

        connection_type connection_;

        on_handshake_type on_handshake_;
        on_error_type on_error_;
        on_timer_type on_timer_;

#if defined BEASTHTTP_CXX17_OPTIONAL
        std::optional<request_parser_type> parser_;
        std::optional<response_serializer_type> serializer_;
#else
        boost::optional<request_parser_type> parser_;
        boost::optional<response_serializer_type> serializer_;
#endif

        buffer_type buffer_;
        queue_type queue_;

    }; // class flesh

    template<class Flesh, context_policy policy>
    class context
    {
        std::shared_ptr<Flesh> flesh_p_;

        bool
        isOk()
        {
            return flesh_p_.operator bool();
        }

    public:

        template<class Handler>
        auto
        advance(typename option::on_error_t arg, Handler&& handler) const -> decltype (
                BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(
                    std::declval<typename option::on_error_t>(), std::declval<Handler>()))
        {
            using handler_type = typename std::decay<Handler>::type;

            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<void (Flesh::*)(
                                typename option::on_error_t, handler_type&, typename std::enable_if<
                                    base::traits::TryInvoke<handler_type,
                                    void(boost::system::error_code,
                                         boost::string_view)>::value, int>::type)>(&Flesh::template member<handler_type>),
                            flesh_p_->shared_from_this(), arg, std::forward<Handler>(handler), 0));
        }

        template<class Handler>
        auto
        advance(typename option::on_timer_t arg, Handler&& handler) const -> decltype (
                BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(
                    std::declval<typename option::on_timer_t>(), std::declval<Handler>()))
        {
            using handler_type = typename std::decay<Handler>::type;

            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<void (Flesh::*)(
                                typename option::on_timer_t, handler_type&, typename std::enable_if<
                                    base::traits::TryInvoke<handler_type,
                                    void(boost::system::error_code,
                                         boost::string_view)>::value, int>::type)>(&Flesh::template member<handler_type>),
                            flesh_p_->shared_from_this(), arg, std::forward<Handler>(handler), 0));
        }

        template<class Handler, class Allocator>
        auto
        advance(typename option::on_error_t arg, Handler&& handler, Allocator&& alloc) const -> decltype (
                BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(
                    std::declval<typename option::on_error_t>(), std::declval<Handler>(), std::declval<Allocator>()))
        {
            using handler_type = typename std::decay<Handler>::type;
            using allocator_type = typename std::decay<Allocator>::type;

            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<void (Flesh::*)(
                                typename option::on_error_t, handler_type&, typename std::enable_if<
                                    base::traits::TryInvoke<handler_type,
                                    void(boost::system::error_code,
                                         boost::string_view)>::value and
                                std::is_constructible<on_error_type, handler_type, allocator_type>::value, int>::type)>(
                                &Flesh::template member<handler_type, allocator_type>),
                            flesh_p_->shared_from_this(), arg, std::forward<Handler>(handler), std::forward<Allocator>(alloc), 0));
        }

        template<class Handler, class Allocator>
        auto
        advance(typename option::on_timer_t arg, Handler&& handler, Allocator&& alloc) const -> decltype (
                BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(
                    std::declval<typename option::on_timer_t>(), std::declval<Handler>(), std::declval<Allocator>()))
        {

            using handler_type = typename std::decay<Handler>::type;
            using allocator_type = typename std::decay<Allocator>::type;

            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<void (Flesh::*)(
                                typename option::on_timer_t, handler_type&,
                                    typename std::enable_if<
                                    base::traits::TryInvoke<handler_type,
                                    void(context_type)>::value and
                                std::is_constructible<on_timer_type, handler_type, allocator_type>::value, int>::type)>(
                                &Flesh::template member<handler_type, allocator_type>),
                            flesh_p_->shared_from_this(), arg, std::forward<Handler>(handler), std::forward<Allocator>(alloc), 0));
        }

        auto
        advance(typename option::get_socket_t arg) const -> decltype (
                BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(
                    std::declval<typename option::get_socket_t>()))
        {
            return flesh_p_->member(arg);
        }

        auto
        advance(typename option::get_ssl_stream_t arg) const -> decltype (
                BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_MEMBER(
                    std::declval<typename option::get_ssl_stream_t>()))
        {
            return flesh_p_->member(arg);
        }

        struct set
        {
            template<class... Args>
            static auto
            on_error(context<Flesh, context_policy::shared> ctx, Args&&... args) -> decltype (
                    BEASTHTTP_REACTOR_SSL_SESSION_CONTEXT_TRY_INVOKE_ADVANCE(on_error_arg, std::declval<Args>()...))
            {
                return ctx.advance(on_error_arg, std::forward<Args>(args)...);
            }

            template<class... Args>
            static auto
            on_timer(context<Flesh, context_policy::shared> ctx, Args&&... args) -> decltype (
                    BEASTHTTP_REACTOR_SSL_SESSION_CONTEXT_TRY_INVOKE_ADVANCE(on_timer_arg, std::declval<Args>()...))
            {
                return ctx.advance(on_timer_arg, std::forward<Args>(args)...);
            }
        };

        struct get
        {
            static auto
            asio_socket(context<Flesh, context_policy::shared> ctx) -> decltype (
                    BEASTHTTP_REACTOR_SSL_SESSION_CONTEXT_TRY_INVOKE_ADVANCE(get_socket_arg))
            {
                return ctx.advance(get_socket_arg);
            }

            static auto
            beast_ssl_stream(context<Flesh, context_policy::shared> ctx) -> decltype (
                    BEASTHTTP_REACTOR_SSL_SESSION_CONTEXT_TRY_INVOKE_ADVANCE(get_socket_arg))
            {
                return ctx.advance(get_socket_arg);
            }
        };

        friend struct set;
        friend struct get;

        context(Flesh& flesh)
            : flesh_p_{flesh.shared_from_this()}
        {
        }

        static context<Flesh, context_policy::shared>
        save(context<Flesh, context_policy::shared> other, bool& is_ok)
        {
            is_ok = other.isOk();

            return other;
        }

        context<Flesh, context_policy::weak>
        weak() const
        {
            return context<Flesh, context_policy::weak>(flesh_p_);
        }

        socket_type&
        asio_socket() const
        {
            return advance(get_socket_arg);
        }

        ssl_stream_type&
        beast_ssl_stream() const
        {
            return advance(get_ssl_stream_arg);
        }

        void
        recv() const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)()>(&Flesh::recv),
                            flesh_p_->shared_from_this()));
        }

        void
        recv(duration_type const duration) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(duration_type const)>(&Flesh::recv),
                            flesh_p_->shared_from_this(), duration));
        }

        void
        recv(time_point_type const time_point) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(time_point_type const)>(&Flesh::recv),
                            flesh_p_->shared_from_this(), time_point));
        }

        template<class _Body>
        void
        send(response_type<_Body>& response) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&)>(&Flesh::send),
                            flesh_p_->shared_from_this(), response));
        }

        template<class _Body>
        void
        send(response_type<_Body>&& response) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&)>(&Flesh::send),
                            flesh_p_->shared_from_this(), std::move(response)));
        }

        template<class _Body>
        void
        send(response_type<_Body>& response,
             duration_type const duration) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&, duration_type const)>(&Flesh::send),
                            flesh_p_->shared_from_this(), response, duration));
        }

        template<class _Body>
        void
        send(response_type<_Body>&& response,
             duration_type const duration) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&, duration_type const)>(&Flesh::send),
                            flesh_p_->shared_from_this(), std::move(response), duration));
        }

        template<class _Body>
        void
        send(response_type<_Body>& response,
             time_point_type const time_point) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&, time_point_type const)>(&Flesh::send),
                            flesh_p_->shared_from_this(), response, time_point));
        }

        template<class _Body>
        void
        send(response_type<_Body>&& response,
             time_point_type const time_point) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&, time_point_type const)>(&Flesh::send),
                            flesh_p_->shared_from_this(), std::move(response), time_point));
        }

        template<class _Body>
        void
        push(response_type<_Body>& response) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&)>(&Flesh::push),
                            flesh_p_->shared_from_this(), response));
        }

        template<class _Body>
        void
        push(response_type<_Body>&& response) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(response_type<_Body>&)>(&Flesh::push),
                            flesh_p_->shared_from_this(), std::move(response)));
        }

        void
        wait() const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)()>(&Flesh::wait),
                            flesh_p_->shared_from_this()));
        }

        void
        wait(duration_type const duration) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(duration_type const)>(&Flesh::wait),
                            flesh_p_->shared_from_this(), duration));
        }

        void
        wait(time_point_type const time_point) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(time_point_type const)>(&Flesh::wait),
                            flesh_p_->shared_from_this(), time_point));
        }

        void
        timer_cancel() const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            &Flesh::timer_cancel, flesh_p_->shared_from_this()));
        }

        void
        eof() const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)()>(&Flesh::eof),
                            flesh_p_->shared_from_this()));
        }

        void
        eof(duration_type const duration) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(duration_type const)>(&Flesh::eof),
                            flesh_p_->shared_from_this(), duration));
        }

        void
        eof(time_point_type const time_point) const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            static_cast<Flesh& (Flesh::*)(time_point_type const)>(&Flesh::eof),
                            flesh_p_->shared_from_this(), time_point));
        }

        void
        force_eof() const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            &Flesh::force_eof, flesh_p_->shared_from_this()));
        }

        void
        force_cls() const
        {
            boost::asio::dispatch(
                        static_cast<base::strand_stream&>(*flesh_p_), std::bind(
                            &Flesh::force_cls, flesh_p_->shared_from_this()));
        }

    }; // class context

    template<class Flesh>
    class context<Flesh, context_policy::weak>
    {
        std::weak_ptr<Flesh> flesh_p_;

    public:

        context(std::shared_ptr<Flesh> flesh_p)
            : flesh_p_{flesh_p}
        {
        }

        bool
        expired() const
        {
            return flesh_p_.expired();
        }

        context<Flesh, context_policy::shared>
        load() const
        {
            return context<Flesh, context_policy::shared>(flesh_p_.lock());
        }

    }; // class context

public:

    template<class Router, class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
            std::declval<context_type>())>::type;

    template<class Deleter, class Allocator, class Router, class... _OnAction>
    static auto
    handshake(allocator_t arg, boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              buffer_type&& buffer, const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
            std::declval<context_type>())>::type;

    template<class Router, class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
            std::declval<context_type>())>::type;

    template<class Deleter, class Allocator, class Router, class... _OnAction>
    static auto
    handshake(allocator_t arg, boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
            std::declval<context_type>())>::type;

    template<class Router, class TimePointOrDuration, class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              TimePointOrDuration const timeOrDuration, buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).handshake(std::declval<TimePointOrDuration>()),
            std::declval<context_type>())>::type;

    template<class Deleter, class Allocator, class Router, class TimePointOrDuration, class... _OnAction>
    static auto
    handshake(allocator_t arg, boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              TimePointOrDuration const timeOrDuration, buffer_type&& buffer,
              const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).handshake(std::declval<TimePointOrDuration>()),
            std::declval<context_type>())>::type;

    template<class Router, class TimePointOrDuration, class... _OnAction>
    static auto
    handshake(boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              TimePointOrDuration const timeOrDuration, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).handshake(std::declval<TimePointOrDuration>()),
            std::declval<context_type>())>::type;

    template<class Deleter, class Allocator, class Router, class TimePointOrDuration, class... _OnAction>
    static auto
    handshake(allocator_t arg, boost::asio::ssl::context& ctx, socket_type&& socket, Router const& router,
              TimePointOrDuration const timeOrDuration, const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
            BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(
                std::declval<Router const&>()).handshake(std::declval<TimePointOrDuration>()),
            std::declval<context_type>())>::type;

    template<class _OnError>
    static auto
    force_eof(boost::asio::ssl::context& ctx, socket_type&& socket, _OnError&& on_error) -> decltype (
            void(BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY()));

    template<class _OnError>
    static auto
    force_cls(boost::asio::ssl::context& ctx, socket_type&& socket, _OnError&& on_error) -> decltype (
            void(BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY()));

}; // class session

namespace _default {

using session_type = session<>;

} // namespace _default
} // namespace ssl
} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#include <http/reactor/ssl/impl/session.hxx>

#endif // not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
