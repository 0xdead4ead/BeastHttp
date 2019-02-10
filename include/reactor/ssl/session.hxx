#if not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
#define BEASTHTTP_REACTOR_SSL_SESSION_HXX

#include "base/cb.hxx"
#include <base/request_processor.hxx>
#include "base/timer.hxx"
#include "base/regex.hxx"

#include "connection.hxx"

#include <boost/beast/core/string.hpp>
#include <boost/system/error_code.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/message.hpp>

#include <regex>
#include <vector>
#include <map>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/asio/steady_timer.hpp>

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

    class context;

    using reference_wrapper = std::reference_wrapper<context const>;

    using resource_regex_type = std::string;

    using resource_type = boost::beast::string_view;

    using method_type = boost::beast::http::verb;

    using body_type = Body;

    using cbexecutor_type = base::cb::executor;

    using request_type = boost::beast::http::request<body_type>;

    using buffer_type = Buffer;

    using connection_type = connection<Protocol, Socket>;

    using socket_type = typename connection_type::socket_type;

    using timer_type = base::timer<Clock, Timer>;

    using duration_type = typename timer_type::duration;

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

        //https://www.boost.org/doc/libs/1_68_0/libs/beast/example/advanced/server/advanced_server.cpp
        class queue
        {
            constexpr static size_t limit = 16;

            struct work
            {
                virtual ~work() = default;
                virtual void operator()() = 0;
            };

            flesh& impl_;
            std::vector<std::unique_ptr<work>> items_;

        public:

            explicit
            queue(flesh& impl)
                : impl_(impl)
            {
                static_assert(limit > 0, "queue limit must be positive");
                items_.reserve(limit);
            }

            // Returns `true` if we have reached the queue limit
            bool
            is_full() const
            {
                return items_.size() >= limit;
            }

            // Called when a message finishes sending
            // Returns `true` if the caller should initiate a read
            bool
            on_write()
            {
                assert(not items_.empty());
                auto const was_full = is_full();
                items_.erase(items_.begin());
                if (not items_.empty())
                    (*items_.front())();
                return was_full;
            }

            // Called by the HTTP handler to send a response.
            template<class Response>
            void
            operator()(Response&& response)
            {
                // This holds a work item
                struct work_impl : work
                {
                    flesh& impl_;
                    std::decay_t<Response> response_;

                    work_impl(flesh& impl, Response&& response)
                        : impl_(impl)
                        , response_(std::forward<Response>(response))
                    {}

                    void
                    operator()()
                    {
                        impl_.do_write(response_);
                    }
                };

                // Allocate and store the work
                items_.push_back(std::make_unique<work_impl>
                                 (impl_, std::forward<Response>(response)));

                // If there was no previous work, start this one
                if (items_.size() == 1)
                    (*items_.front())();
            }

        }; // class queue

    public:

        template<class>
        friend class __gnu_cxx::new_allocator;

        connection_type&
        getConnection()
        {
            return connection_;
        }

        flesh&
        handshake()
        {
            do_handshake();

            return *this;
        }

        flesh&
        handshake(duration_type const& duration)
        {
            timer_.stream().expires_after(duration);

            return handshake();
        }

        flesh&
        handshake(time_point_type const& time_point)
        {
            timer_.stream().expires_at(time_point);

            return handshake();
        }

        flesh&
        recv()
        {
            request_ = {};

            do_read();

            return *this;
        }

        flesh&
        recv(duration_type const& duration)
        {
            timer_.stream().expires_after(duration);

            return recv();
        }

        flesh&
        recv(time_point_type const& time_point)
        {
            timer_.stream().expires_at(time_point);

            return recv();
        }

        template<class Response>
        flesh&
        send(Response&& response)
        {
            queue_(std::forward<Response>(response));

            return *this;
        }

        template<class Response>
        flesh&
        send(Response&& response, duration_type const& duration)
        {
            timer_.stream().expires_after(duration);

            return send(std::forward<Response>(response));
        }

        template<class Response>
        flesh&
        send(Response&& response, time_point_type const& time_point)
        {
            timer_.stream().expires_at(time_point);

            return send(std::forward<Response>(response));
        }

        flesh&
        eof()
        {
            do_eof();

            return *this;
        }

        flesh&
        eof(duration_type const& duration)
        {
            timer_.stream().expires_after(duration);

            return eof();
        }

        flesh&
        eof(time_point_type const& time_point)
        {
            timer_.stream().expires_at(time_point);

            return eof();
        }

        flesh&
        launch_timer()
        {
            timer_.async_wait(
                        std::bind(
                            &flesh::on_timer,
                            this->shared_from_this(),
                            std::placeholders::_1));

            return *this;
        }

    private:

        template<class _OnHandshake>
        explicit
        flesh(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              buffer_type&& buffer,
              _OnHandshake&& on_handshake)
            : base_type{resource_map, method_map, flags},
              connection_{std::move(socket), ctx},
              timer_{socket.get_executor(), (time_point_type::max)()},
              on_handshake_{std::forward<_OnHandshake>(on_handshake)},
              buffer_{std::move(buffer)},
              queue_{*this}
        {}

        template<class _OnHandshake, class _OnError>
        explicit
        flesh(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              buffer_type&& buffer,
              _OnHandshake&& on_handshake,
              _OnError&& on_error)
            : base_type{resource_map, method_map, flags},
              connection_{std::move(socket), ctx},
              timer_{socket.get_executor(), (time_point_type::max)()},
              on_handshake_{std::forward<_OnHandshake>(on_handshake)},
              on_error_{std::forward<_OnError>(on_error)},
              buffer_{std::move(buffer)},
              queue_{*this}
        {}

        template<class _OnHandshake, class _OnError, class _OnTimer>
        explicit
        flesh(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              buffer_type&& buffer,
              _OnHandshake&& on_handshake,
              _OnError&& on_error, _OnTimer&& on_timer)
            : base_type{resource_map, method_map, flags},
              connection_{std::move(socket), ctx},
              timer_{socket.get_executor(), (time_point_type::max)()},
              on_handshake_{std::forward<_OnHandshake>(on_handshake)},
              on_error_{std::forward<_OnError>(on_error)},
              on_timer_{std::forward<_OnTimer>(on_timer)},
              buffer_{std::move(buffer)},
              queue_{*this}
        {}

        flesh&
        force_eof()
        {
            do_force_eof();

            return *this;
        }

        void
        on_timer(boost::system::error_code ec)
        {
            context _self{*this};

            if (ec and ec != boost::asio::error::operation_aborted
                    and on_error_)
                return on_error_(ec, "async_wait/on_timer");

            // Verify that the timer really expired since the deadline may have moved.
            if (timer_.stream().expiry() <= clock_type::now()) {

                bool is_alive = connection_.stream().next_layer().is_open();

                if (on_timer_ and is_alive)
                    return on_timer_(std::cref(_self));

                if (not is_alive)
                    return;

                do_timeout();
            }
        }

        void
        on_handshake(boost::system::error_code ec,
                     std::size_t bytes_used)
        {
            context _self{*this};

            if (ec and on_error_)
                return on_error_(ec, "async_handshake/on_handshake");

            buffer_.consume(bytes_used);

            on_handshake_(std::cref(_self));
        }

        void
        on_shutdown(boost::system::error_code ec)
        {
            // Happens when the shutdown times out
            if (ec == boost::asio::error::operation_aborted)
                return;

            if (ec and on_error_)
                return on_error_(ec, "async_shutdown/on_shutdown");
        }

        void
        on_read(boost::system::error_code ec,
                std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if (ec == boost::beast::http::error::end_of_stream) {
                do_eof();
                return;
            }

            if (ec and on_error_)
                return on_error_(ec, "async_read/on_read");

            process_request();
        }

        void
        on_write(boost::system::error_code ec,
                 std::size_t bytes_transferred, bool close)
        {
            boost::ignore_unused(bytes_transferred);

            if (ec and on_error_)
                return on_error_(ec, "async_write/on_write");

            if (close) {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                do_eof();
                return;
            }

            // Inform the queue that a write completed
            if (queue_.on_write())
                // Read another request
                recv();
        }

        void
        do_handshake()
        {
            connection_.async_handshake(
                        buffer_,
                        std::bind(&flesh::on_handshake,
                                  this->shared_from_this(),
                                  std::placeholders::_1,
                                  std::placeholders::_2));
        }

        void
        do_shutdown()
        {
            connection_.async_shutdown(std::bind(&flesh::on_shutdown,
                                                 this->shared_from_this(),
                                                 std::placeholders::_1));
        }

        template<class Response>
        void
        do_write(Response& response)
        {
            connection_.async_write(
                        response,
                        std::bind(&flesh::on_write, this->shared_from_this(),
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  response.need_eof()));
        }

        void
        do_read()
        {
            connection_.async_read(
                        buffer_,
                        request_,
                        std::bind(&flesh::on_read, this->shared_from_this(),
                                  std::placeholders::_1,
                                  std::placeholders::_2));
        }

        void
        do_eof()
        {
            eof_ = true;

            do_shutdown();
        }

        void
        do_force_eof()
        {
            // Is this connection alive?
            if (not connection_.stream().next_layer().is_open())
                return;

            auto ec = connection_.force_shutdown(shutdown_type::shutdown_send);
            if (ec and on_error_)
                on_error_(ec, "shutdown/force_eof");
        }

        void
        do_timeout()
        {
            if (eof_)
                return;

            do_eof();
        }

        void
        process_request()
        {
            this->provide(request_, *this);

            // If we aren't at the queue limit, try to pipeline another request
            if (not queue_.is_full() and connection_.stream().next_layer().is_open())
                recv();
        }

        bool eof_ = false;

        on_handshake_type on_handshake_;
        on_error_type on_error_;
        on_timer_type on_timer_;

        timer_type timer_;

        connection_type connection_;
        request_type request_;
        buffer_type buffer_;
        queue queue_;

    }; // class flesh

    class context
    {
        flesh& impl_;

    public:

        context(flesh& impl)
            : impl_{impl}
        {}

        connection_type&
        getConnection() const &
        {
            return impl_.getConnection();
        }

        context&
        recv() const &
        {
            impl_.recv();
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

        template<class TimeOrDuration>
        context&
        recv(TimeOrDuration const& time_or_duration) const &
        {
            impl_.recv(time_or_duration);
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

        template<class Response>
        context&
        send(Response&& response) const &
        {
            impl_.send(std::forward<Response>(response));
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

        template<class Response, class TimeOrDuration>
        context&
        send(Response&& response, TimeOrDuration const& time_or_duration) const &
        {
            impl_.send(std::forward<Response>(response), time_or_duration);
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

        context&
        eof() const &
        {
            impl_.eof();
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

        template<class TimeOrDuration>
        context&
        eof(TimeOrDuration const& time_or_duration) const &
        {
            impl_.eof(time_or_duration);
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

        context&
        launch_timer() const &
        {
            impl_.launch_timer();
            return const_cast<typename std::add_lvalue_reference<context>::type>(*this);
        }

    }; // class context

    template<class... _OnAction>
    static flesh&
    handshake(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              buffer_type&& buffer,
              _OnAction&&... on_action)
    {
        return std::make_shared<flesh>(
                    ctx, std::move(socket), resource_map, method_map, flags, std::move(buffer),
                    std::forward<_OnAction>(on_action)...)->handshake();
    }

    template<class... _OnAction>
    static flesh&
    handshake(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              _OnAction&&... on_action)
    {
        buffer_type buffer;
        return handshake(ctx, std::move(socket), resource_map, method_map, flags, std::move(buffer), std::forward<_OnAction>(on_action)...);
    }

    template<class... _OnAction>
    static flesh&
    handshake(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              duration_type const& duration,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              buffer_type&& buffer,
              _OnAction&&... on_action)
    {
        return std::make_shared<flesh>(
                    ctx, std::move(socket), resource_map, method_map, flags, std::move(buffer),
                    std::forward<_OnAction>(on_action)...)->handshake(duration);
    }

    template<class... _OnAction>
    static flesh&
    handshake(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              duration_type const& duration,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              _OnAction&&... on_action)
    {
        buffer_type buffer;
        return handshake(ctx, std::move(socket), duration, resource_map, method_map,
                         flags, std::move(buffer), std::forward<_OnAction>(on_action)...);
    }

    template<class... _OnAction>
    static flesh&
    handshake(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              time_point_type const& time_point,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              buffer_type&& buffer,
              _OnAction&&... on_action)
    {
        return std::make_shared<flesh>(
                    ctx, std::move(socket), resource_map, method_map, flags, std::move(buffer),
                    std::forward<_OnAction>(on_action)...)->handshake(time_point);
    }

    template<class... _OnAction>
    static flesh&
    handshake(boost::asio::ssl::context& ctx,
              socket_type&& socket,
              time_point_type const& time_point,
              std::shared_ptr<resource_map_type> const& resource_map,
              std::shared_ptr<method_map_type> const& method_map,
              regex_flag_type flags,
              _OnAction&&... on_action)
    {
        buffer_type buffer;
        return handshake(ctx, std::move(socket), time_point, resource_map, method_map,
                         flags, std::move(buffer), std::forward<_OnAction>(on_action)...);
    }

    template<class... _OnAction>
    static flesh&
    eof(boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        _OnAction&&... on_action)
    {
        buffer_type buffer;
        return std::make_shared<flesh>(
                    ctx, std::move(socket), resource_map, method_map, flags, std::move(buffer),
                    std::forward<_OnAction>(on_action)...)->force_eof();
    }

}; // class session

namespace _default {

using session_type = session<>;

} // namespace _default
} // namespace ssl
} // namespace reactor
} // namespace http

#endif // not defined BEASTHTTP_REACTOR_SSL_SESSION_HXX
