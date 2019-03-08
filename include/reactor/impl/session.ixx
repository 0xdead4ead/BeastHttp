#if not defined BEASTHTTP_REACTOR_SESSION_IXX
#define BEASTHTTP_REACTOR_SESSION_IXX

#define BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE \
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
             template<typename> class OnTimer>

#define BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES \
    Body, Buffer, Protocol, Socket, Clock, Timer, Entry, Container, MethodMap, ResourceMap, OnError, OnTimer


namespace _0xdead4ead {
namespace http {
namespace reactor {

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::connection_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::getConnection()
{
    return connection_;
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
        duration_type const& duration)
{
    timer_.stream().expires_after(duration);

    return recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        time_point_type const& time_point)
{
    timer_.stream().expires_at(time_point);

    return recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        Response&& response)
{
    queue_(std::forward<Response>(response));

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        Response&& response, duration_type const& duration)
{
    timer_.stream().expires_after(duration);

    return send(std::forward<Response>(response));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        Response&& response, time_point_type const& time_point)
{
    timer_.stream().expires_at(time_point);

    return send(std::forward<Response>(response));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::eof()
{
    do_eof();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::cls()
{
    do_cls();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::launch_timer()
{
    timer_.async_wait(
                std::bind(
                    &flesh::on_timer,
                    this->shared_from_this(),
                    std::placeholders::_1));

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        connection_type&& connection,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer)
    : base_type{resource_map, method_map, flags},
      connection_{std::move(connection)},
      timer_{connection.stream().get_executor(), (time_point_type::max)()},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _OnError>
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        connection_type&& connection,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnError&& on_error)
    : base_type{resource_map, method_map, flags},
      connection_{std::move(connection)},
      timer_{connection.stream().get_executor(), (time_point_type::max)()},
      on_error_{std::forward<_OnError>(on_error)},
      buffer_{std::move(buffer)},
      queue_{*this}
{
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _OnError, class _OnTimer>
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::flesh(
        connection_type&& connection,
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        regex_flag_type flags,
        buffer_type&& buffer,
        _OnError&& on_error, _OnTimer&& on_timer)
    : base_type{resource_map, method_map, flags},
      connection_{std::move(connection)},
      timer_{connection.stream().get_executor(), (time_point_type::max)()},
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

    if (ec and ec != boost::asio::error::operation_aborted
            and on_error_)
        return on_error_(ec, "async_wait/on_timer");

    if (timer_.stream().expiry() <= clock_type::now()) {

        bool is_alive = connection_.stream().is_open();

        if (on_timer_ and is_alive)
            return on_timer_(std::cref(_self));

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

    if (ec and on_error_)
        return on_error_(ec, "async_read/on_read");

    process_request();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::on_write(
        boost::system::error_code ec, std::size_t bytes_transferred, bool close)
{
    boost::ignore_unused(bytes_transferred);

    if (ec and on_error_)
        return on_error_(ec, "async_write/on_write");

    if (close) {
        do_eof();
        return;
    }

    if (queue_.on_write())
        recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_write(
        Response& response)
{
    connection_.async_write(
                response,
                std::bind(&flesh::on_write, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2,
                          response.need_eof()));
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
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::process_request()
{
    this->provide(request_, *this);

    if (not queue_.is_full() and connection_.stream().is_open())
        recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     buffer_type&& buffer,
     _OnAction&&... on_action)
{
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     _OnAction&&... on_action)
{
    buffer_type buffer;
    return recv(std::move(socket), resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     duration_type const& duration,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     buffer_type&& buffer,
     _OnAction&&... on_action)
{
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->recv(duration);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     duration_type const& duration,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     _OnAction&&... on_action)
{
    buffer_type buffer;
    return recv(std::move(socket), duration, resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     time_point_type const& time_point,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     buffer_type&& buffer,
     _OnAction&&... on_action)
{
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->recv(time_point);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     time_point_type const& time_point,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     _OnAction&&... on_action)
{
    buffer_type buffer;
    return recv(std::move(socket), time_point, resource_map, method_map, flags,
                std::move(buffer), std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response, class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(Response&& response,
     socket_type&& socket,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     buffer_type&& buffer,
     _OnAction&&... on_action)
{
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->send(std::forward<Response>(response));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response, class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(Response&& response,
     socket_type&& socket,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     _OnAction&&... on_action)
{
    buffer_type buffer;
    return send(std::forward<Response>(response), std::move(socket),
                resource_map, method_map, flags, std::move(buffer),
                std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response, class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(Response&& response,
     socket_type&& socket,
     duration_type const& duration,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     buffer_type&& buffer,
     _OnAction&&... on_action)
{
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->send(std::forward<Response>(response), duration);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response, class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(Response&& response,
     socket_type&& socket,
     duration_type const& duration,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     _OnAction&&... on_action)
{
    buffer_type buffer;
    return send(std::forward<Response>(response), std::move(socket),
                duration, resource_map, method_map, flags, std::move(buffer),
                std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response, class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(Response&& response,
     socket_type&& socket,
     time_point_type const& time_point,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     buffer_type&& buffer,
     _OnAction&&... on_action)
{
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->send(std::forward<Response>(response), time_point);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Response, class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(Response&& response,
     socket_type&& socket,
     time_point_type const& time_point,
     std::shared_ptr<resource_map_type> const& resource_map,
     std::shared_ptr<method_map_type> const& method_map,
     regex_flag_type flags,
     _OnAction&&... on_action)
{
    buffer_type buffer;
    return send(std::forward<Response>(response), std::move(socket), time_point,
                resource_map, method_map, flags, std::move(buffer),
                std::forward<_OnAction>(on_action)...);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class... _OnAction>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::eof(socket_type&& socket,
    std::shared_ptr<resource_map_type> const& resource_map,
    std::shared_ptr<method_map_type> const& method_map,
    regex_flag_type flags,
    _OnAction&&... on_action)
{
    buffer_type buffer;
    return std::make_shared<flesh_type>(
                connection_type{std::move(socket)}, resource_map, method_map,
                flags, std::move(buffer), std::forward<_OnAction>(on_action)...)
            ->eof();
}

} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_SESSION_IXX
