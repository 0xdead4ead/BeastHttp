#if not defined BEASTHTTP_REACTOR_IMPL_SESSION_HXX
#define BEASTHTTP_REACTOR_IMPL_SESSION_HXX

#define BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE \
    template<class Body, \
             class Fields, \
             class Buffer, \
             class Protocol, \
             template<typename> class Socket, \
             class Clock, \
             template<typename, typename...> class Timer, \
             template<typename> class Entry, \
             template<typename, typename...> class Container, \
             template<typename, typename, typename...> class MethodMap, \
             template<typename, typename, typename...> class ResourceMap, \
             template<typename> class OnError, \
             template<typename> class OnTimer>

#define BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES \
    Body, Fields, Buffer, Protocol, Socket, Clock, Timer, Entry, Container, MethodMap, ResourceMap, OnError, OnTimer

namespace _0xdead4ead {
namespace http {
namespace reactor {

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::socket_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::asio_socket()
{
    return get_asio_socket();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv()
{
    request_ = {};

    do_read();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body, _Fields>& response)
{
    queue_(response);

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body, _Fields>&& response)
{
    queue_(std::move(response));

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body, _Fields>& response, duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return send(response);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body, _Fields>&& response, duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return send(std::move(response));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body, _Fields>& response, time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return send(response);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body, _Fields>&& response, time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return send(std::move(response));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::push(
        response_type<_Body, _Fields>& response)
{
    do_push(response);

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::push(
        response_type<_Body, _Fields>&& response)
{
    auto response_{std::move(response)};

    do_push(response_);

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::wait()
{
    timer_.stream().expires_at((time_point_type::max)());

    do_launch_timer();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::wait(
        duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::wait(
        time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::timer_cancel()
{
    do_timer_cancel();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::eof()
{
    do_eof();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::cls()
{
    do_cls();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        mutex_type* mutex,
        buffer_type&& buffer)
    : base::strand_stream{socket.get_executor()},
      base_type{resource_map, method_map, flags},
      router_mutex_{mutex},
      timer_{static_cast<base::strand_stream&>(*this), (time_point_type::max)()},
      connection_{std::move(socket), static_cast<base::strand_stream&>(*this)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _OnError>
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        mutex_type* mutex,
        buffer_type&& buffer,
        _OnError&& on_error,
        typename std::enable_if<
        base::traits::TryInvoke<_OnError,
        void(boost::system::error_code,
             boost::string_view)>::value, int>::type)
    : base::strand_stream{socket.get_executor()},
      base_type{resource_map, method_map, flags},
      router_mutex_{mutex},
      timer_{static_cast<base::strand_stream&>(*this), (time_point_type::max)()},
      connection_{std::move(socket), static_cast<base::strand_stream&>(*this)},
      on_error_{std::forward<_OnError>(on_error)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _OnError, class _OnTimer>
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        mutex_type* mutex,
        buffer_type&& buffer,
        _OnError&& on_error, _OnTimer&& on_timer,
        typename std::enable_if<
        base::traits::TryInvoke<_OnError,
        void(boost::system::error_code,
             boost::string_view)>::value and
        base::traits::TryInvoke<_OnTimer,
        void(context_type)>::value, int>::type)
    : base::strand_stream{socket.get_executor()},
      base_type{resource_map, method_map, flags},
      router_mutex_{mutex},
      timer_{static_cast<base::strand_stream&>(*this), (time_point_type::max)()},
      connection_{std::move(socket), static_cast<base::strand_stream&>(*this)},
      on_error_{std::forward<_OnError>(on_error)},
      on_timer_{std::forward<_OnTimer>(on_timer)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::on_timer(
        boost::system::error_code ec)
{
    context_type _self{*this};

    if (ec and ec != boost::asio::error::operation_aborted) {
        if (on_error_)
            on_error_(ec, "async_wait/on_timer");

        return;
    }

    if (timer_.stream().expiry() <= clock_type::now()) {

        bool is_alive = connection_.stream().is_open();

        if (on_timer_ and is_alive) {
            on_timer_(std::move(_self));
            return;
        }

        if (not is_alive)
            return;

        do_timeout();
    }
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::on_read(
        boost::system::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec == boost::beast::http::error::end_of_stream) {
        do_eof();
        return;
    }

    if (ec) {
        if (on_error_)
            on_error_(ec, "async_read/on_read");

        do_timer_cancel();
        return;
    }

    do_process_request();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::on_write(
        boost::system::error_code ec, std::size_t bytes_transferred, bool close)
{
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        if (on_error_)
            on_error_(ec, "async_write/on_write");

        do_timer_cancel();
        return;
    }

    if (close) {
        do_eof();
        return;
    }

    if (queue_.on_write())
        recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_write(
        response_type<_Body, _Fields>& response)
{
    connection_.async_write(
                response,
                std::bind(&flesh::on_write, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2,
                          response.need_eof()));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body, class _Fields>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_push(
        response_type<_Body, _Fields>& response)
{
    auto ec = connection_.write(response);

    if (ec) {
        if (on_error_)
            on_error_(ec, "write/do_push");

        do_timer_cancel();
        return;
    }
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_read()
{
    connection_.async_read(
                buffer_,
                request_,
                std::bind(&flesh::on_read, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_eof()
{
    if (not connection_.stream().is_open())
        return;

    auto ec = connection_.shutdown(shutdown_type::shutdown_send);
    if (ec and on_error_)
        on_error_(ec, "shutdown/eof");

    do_timer_cancel();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_cls()
{
    if (not connection_.stream().is_open())
        return;

    auto ec = connection_.close();
    if (ec and on_error_)
        on_error_(ec, "close/cls");

    do_timer_cancel();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_launch_timer()
{
    timer_.async_wait(
                std::bind(
                    &flesh::on_timer,
                    this->shared_from_this(),
                    std::placeholders::_1));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_timeout()
{
    auto ec = connection_.shutdown(shutdown_type::shutdown_both);
    if (ec and on_error_)
        on_error_(ec, "shutdown/on_timer");

    ec = connection_.close();
    if (ec and on_error_)
        on_error_(ec, "close/on_timer");
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_process_request()
{
    {
        BEASTHTTP_LOCKABLE_ENTER_TO_READ(*router_mutex_)
        this->provide(request_, *this);
    }

    if (not queue_.is_full() and connection_.stream().is_open())
        recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::socket_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::get_asio_socket()
{
    return connection_.stream();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_timer_cancel()
{
    auto ec = timer_.cancel();

    if (ec and on_error_)
        on_error_(ec, "cancel/do_timer_cancel");
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     Router const& router, buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    return std::make_shared<flesh_type>(
                    std::move(socket), router.resource_map(), router.method_map(),
                    router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...)
                ->recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     Router const& router, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;
    return recv(std::move(socket), router, std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     Router const& router, TimePointOrDuration const timeOrDuration,
     buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).recv(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    return std::make_shared<flesh_type>(
                    std::move(socket), router.resource_map(), router.method_map(),
                    router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...)
                ->recv(timeOrDuration);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     Router const& router, TimePointOrDuration const timeOrDuration,
     _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).recv(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;
    return recv(std::move(socket), router, timeOrDuration,
                std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class Response, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(socket_type&& socket,
     Response&& response, Router const& router, buffer_type&& buffer,
     _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>()),
        std::declval<context_type>())>::type
{
    return std::make_shared<flesh_type>(
                    std::move(socket), router.resource_map(), router.method_map(),
                    router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...)
                ->send(std::forward<Response>(response));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class Response, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(socket_type&& socket,
     Response&& response, Router const& router, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;
    return send(std::move(socket), std::forward<Response>(response), router,
                std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class Response, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(socket_type&& socket,
     Response&& response, Router const& router, TimePointOrDuration const timeOrDuration,
     buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>(), std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    return std::make_shared<flesh_type>(
                    std::move(socket), router.resource_map(), router.method_map(),
                    router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...)
                ->send(std::forward<Response>(response), timeOrDuration);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class Response, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(socket_type&& socket,
     Response&& response, Router const& router, TimePointOrDuration const timeOrDuration,
     _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>(), std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;
    return send(std::move(socket), std::forward<Response>(response), router,
                timeOrDuration, std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(socket_type&& socket,
     Router const& router, buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    return std::make_shared<flesh_type>(
                    std::move(socket), router.resource_map(), router.method_map(),
                    router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...)
                ->wait();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(socket_type&& socket,
     Router const& router, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;
    return wait(std::move(socket), router, std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(socket_type&& socket,
     Router const& router, TimePointOrDuration const timeOrDuration,
     buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).wait(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    return std::make_shared<flesh_type>(
                std::move(socket), router.resource_map(), router.method_map(),
                router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->wait(timeOrDuration);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(socket_type&& socket,
     Router const& router, TimePointOrDuration const timeOrDuration,
     _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).wait(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;
    return wait(std::move(socket), router, timeOrDuration,
                std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::eof(socket_type&& socket,
    _OnAction&&... on_action) -> decltype (void(
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY()))
{
    buffer_type buffer;
    return flesh_type(std::move(socket), {}, {}, {}, {}, std::move(buffer),
                      std::forward<_OnAction>(on_action)...).eof();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::cls(socket_type&& socket,
    _OnAction&&... on_action) -> decltype (void(
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY()))
{
    buffer_type buffer;
    return flesh_type(std::move(socket), {}, {}, {}, {}, std::move(buffer),
                      std::forward<_OnAction>(on_action)...).cls();
}

} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_IMPL_SESSION_HXX
