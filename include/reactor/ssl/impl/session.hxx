#if not defined BEASTHTTP_REACTOR_SSL_IMPL_SESSION_HXX
#define BEASTHTTP_REACTOR_SSL_IMPL_SESSION_HXX

#define BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE \
    template<class Body, \
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
             template<typename> class OnTimer, \
             template<typename> class OnHandshake>

#define BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES \
    Body, Buffer, Protocol, Socket, Clock, Timer, Entry, Container, MethodMap, ResourceMap, OnError, OnTimer, OnHandshake

namespace _0xdead4ead {
namespace http {
namespace reactor {
namespace ssl {

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>
::socket_type&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::asio_socket()
{
    return get_asio_socket();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>
::ssl_stream_type&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::asio_ssl_stream()
{
    return get_asio_ssl_stream();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::handshake()
{
    do_handshake();

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::handshake(
        duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return handshake();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::handshake(
        time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return handshake();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::recv()
{
    request_ = {};

    do_read();

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return recv();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return recv();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_OtherBody>& response)
{
    queue_(response);

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_OtherBody>&& response)
{
    queue_(std::move(response));

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_OtherBody>& response, duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return send(response);
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_OtherBody>&& response, duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    return send(std::move(response));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_OtherBody>& response, time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return send(response);
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_OtherBody>&& response, time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    return send(std::move(response));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::eof()
{
    do_eof();

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::eof(
        duration_type const duration)
{
    timer_.stream().expires_after(duration);

    return eof();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::eof(
        time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    return eof();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::force_eof()
{
    do_force_eof();

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::force_cls()
{
    do_force_cls();

    return *this;
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OnHandshake>
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnHandshake&& on_handshake,
        typename std::enable_if<
        base::traits::TryInvoke<_OnHandshake,
        void(context_type)>::value, int>::type)
    : base_type{resource_map, method_map, flags},
      connection_{std::move(socket), ctx},
      timer_{socket.get_executor(), (time_point_type::max)()},
      on_handshake_{std::forward<_OnHandshake>(on_handshake)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OnHandshake, class _OnError>
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnHandshake&& on_handshake,
        _OnError&& on_error,
        typename std::enable_if<
        base::traits::TryInvoke<_OnHandshake,
        void(context_type)>::value and
        base::traits::TryInvoke<_OnError,
        void(boost::system::error_code,
             boost::string_view)>::value, int>::type)
    : base_type{resource_map, method_map, flags},
      connection_{std::move(socket), ctx},
      timer_{socket.get_executor(), (time_point_type::max)()},
      on_handshake_{std::forward<_OnHandshake>(on_handshake)},
      on_error_{std::forward<_OnError>(on_error)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OnHandshake, class _OnError, class _OnTimer>
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnHandshake&& on_handshake,
        _OnError&& on_error, _OnTimer&& on_timer,
        typename std::enable_if<
        base::traits::TryInvoke<_OnHandshake,
        void(context_type)>::value and
        base::traits::TryInvoke<_OnError,
        void(boost::system::error_code,
             boost::string_view)>::value and
        base::traits::TryInvoke<_OnTimer,
        void(context_type)>::value, int>::type)
    : base_type{resource_map, method_map, flags},
      connection_{std::move(socket), ctx},
      timer_{socket.get_executor(), (time_point_type::max)()},
      on_handshake_{std::forward<_OnHandshake>(on_handshake)},
      on_error_{std::forward<_OnError>(on_error)},
      on_timer_{std::forward<_OnTimer>(on_timer)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::on_timer(
        boost::system::error_code ec)
{
    context_type _self{*this};

    if (ec and ec != boost::asio::error::operation_aborted) {
        if (on_error_)
            on_error_(ec, "async_wait/on_timer");

        return;
    }

    if (timer_.stream().expiry() <= clock_type::now()) {

        bool is_alive = connection_.stream().next_layer().is_open();

        if (on_timer_ and is_alive) {
            on_timer_(std::move(_self));
            return;
        }

        if (not is_alive)
            return;

        do_timeout();
    }
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::on_handshake(
        boost::system::error_code ec, std::size_t bytes_used)
{
    context_type _self{*this};

    if (ec) {
        if (on_error_)
            on_error_(ec, "async_handshake/on_handshake");

        return;
    }

    buffer_.consume(bytes_used);

    on_handshake_(std::move(_self));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::on_shutdown(
        boost::system::error_code ec)
{
    if (ec == boost::asio::error::operation_aborted)
        return;

    if (ec and on_error_) {
        on_error_(ec, "async_shutdown/on_shutdown");
        return;
    }
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::on_read(
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

        return;
    }

    do_process_request();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::on_write(
        boost::system::error_code ec, std::size_t bytes_transferred, bool close)
{
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        if (on_error_)
            on_error_(ec, "async_write/on_write");

        return;
    }

    if (close) {
        do_eof();
        return;
    }

    if (queue_.on_write())
        recv();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_handshake()
{
    connection_.async_handshake(
                buffer_,
                std::bind(&flesh::on_handshake,
                          this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_shutdown()
{
    connection_.async_shutdown(
                std::bind(&flesh::on_shutdown,
                          this->shared_from_this(),
                          std::placeholders::_1));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class _OtherBody>
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_write(
        response_type<_OtherBody>& response)
{
    connection_.async_write(
                response,
                std::bind(&flesh::on_write, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2,
                          response.need_eof()));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_read()
{
    connection_.async_read(
                buffer_,
                request_,
                std::bind(&flesh::on_read, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_eof()
{
    eof_ = true;

    do_shutdown();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_force_eof()
{
    if (not connection_.stream().next_layer().is_open())
        return;

    auto ec = connection_.force_shutdown(shutdown_type::shutdown_send);
    if (ec and on_error_)
        on_error_(ec, "shutdown/force_eof");
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_force_cls()
{
    if (not connection_.stream().next_layer().is_open())
        return;

    auto ec = connection_.force_close();
    if (ec and on_error_)
        on_error_(ec, "close/force_cls");
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_launch_timer()
{
    timer_.async_wait(
                std::bind(
                    &flesh::on_timer,
                    this->shared_from_this(),
                    std::placeholders::_1));
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_timeout()
{
    if (eof_)
        return;

    do_eof();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::do_process_request()
{
    this->provide(request_, *this);

    if (not queue_.is_full() and connection_.stream().next_layer().is_open())
        recv();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>
::socket_type&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::get_asio_socket()
{
    return connection_.socket();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>
::ssl_stream_type&
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::flesh::get_asio_ssl_stream()
{
    return connection_.stream();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::handshake(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnAction&&... on_action) -> decltype (
        flesh_type(std::declval<boost::asio::ssl::context&>(),
                   std::declval<socket_type>(),
                   std::declval<std::shared_ptr<resource_map_type>>(),
                   std::declval<std::shared_ptr<method_map_type>>(),
                   std::declval<regex_flag_type>(),
                   std::declval<buffer_type>(),
                   std::declval<_OnAction>()...),
        std::declval<flesh_type&>())
{
    return std::make_shared<flesh_type>(
                ctx, std::move(socket), resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->handshake();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::handshake(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        _OnAction&&... on_action) -> decltype (
        BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
        std::declval<flesh_type&>())
{
    buffer_type buffer;
    return handshake(ctx, std::move(socket), resource_map, method_map, flags,
                     std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::handshake(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        TimePointOrDuration const timeOrDuration,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnAction&&... on_action) -> decltype (
        BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE().handshake(std::declval<TimePointOrDuration>()),
        std::declval<flesh_type&>())
{
    return std::make_shared<flesh_type>(
                ctx, std::move(socket), resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->handshake(timeOrDuration);
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::handshake(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        TimePointOrDuration const timeOrDuration,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        _OnAction&&... on_action) -> decltype (
        BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE().handshake(std::declval<TimePointOrDuration>()),
        std::declval<flesh_type&>())
{
    buffer_type buffer;
    return handshake(ctx, std::move(socket), timeOrDuration, resource_map,
                     method_map, flags, std::move(buffer),
                     std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::force_eof(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        _OnAction&&... on_action) -> decltype (
        BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
        std::declval<flesh_type&>())
{
    buffer_type buffer;
    return std::make_shared<flesh_type>(
                ctx, std::move(socket), resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->force_eof();
}

BEASTHTTP_REACTOR_SSL_SESSION_TMPL_DECLARE
template<class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SSL_SESSION_TMPL_ATTRIBUTES>::force_cls(
        boost::asio::ssl::context& ctx,
        socket_type&& socket,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        _OnAction&&... on_action) -> decltype (
        BEASTHTTP_REACTOR_SSL_SESSION_TRY_INVOKE_FLESH_TYPE(),
        std::declval<flesh_type&>())
{
    buffer_type buffer;
    return std::make_shared<flesh_type>(
                ctx, std::move(socket), resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->force_cls();
}

} // namespace ssl
} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_SSL_IMPL_SESSION_HXX
