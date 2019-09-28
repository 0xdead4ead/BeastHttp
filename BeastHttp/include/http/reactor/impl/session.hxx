#if not defined BEASTHTTP_REACTOR_IMPL_SESSION_HXX
#define BEASTHTTP_REACTOR_IMPL_SESSION_HXX

#define BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE \
    template<class Body, \
             class RequestParser, \
             class Buffer, \
             class Protocol, \
             class Socket, \
             class Clock, \
             class Timer, \
             template<typename> class Entry, \
             template<typename, typename...> class Container, \
             template<typename, typename, typename...> class MethodMap, \
             template<typename, typename, typename...> class ResourceMap, \
             template<typename> class OnError, \
             template<typename> class OnTimer>

#define BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES \
    Body, RequestParser, Buffer, Protocol, Socket, Clock, Timer, Entry, Container, MethodMap, ResourceMap, OnError, OnTimer

namespace _0xdead4ead {
namespace http {
namespace reactor {

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv()
{
    parser_.emplace();

    do_read();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        duration_type const duration)
{
    parser_.emplace();

    timer_.stream().expires_after(duration);

    do_launch_timer();

    do_read();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::recv(
        time_point_type const time_point)
{
    parser_.emplace();

    timer_.stream().expires_at(time_point);

    do_launch_timer();

    do_read();

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body>& response)
{
    queue_(response);

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body>& response, duration_type const duration)
{
    timer_.stream().expires_after(duration);

    do_launch_timer();

    queue_(response);

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::send(
        response_type<_Body>& response, time_point_type const time_point)
{
    timer_.stream().expires_at(time_point);

    do_launch_timer();

    queue_(response);

    return *this;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body>
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::push(
        response_type<_Body>& response)
{
    do_push(response);

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
    do_eof(shutdown_type::shutdown_send);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::cls()
{
    do_cls();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Handler>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::member(
        typename option::on_error_t, Handler& handler,
        typename std::enable_if<
        base::traits::TryInvoke<Handler,
        void(boost::system::error_code,
             boost::string_view)>::value, int>::type)
{
    on_error_ = on_error_type(std::move(handler));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Handler>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::member(
        typename option::on_timer_t, Handler& handler,
        typename std::enable_if<
        base::traits::TryInvoke<Handler,
        void(context_type)>::value, int>::type)
{
    on_timer_ = on_timer_type(std::move(handler));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Handler, class Allocator>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::member(
        typename option::on_error_t, Handler& handler, const Allocator& alloc,
        typename std::enable_if<
        base::traits::TryInvoke<Handler,
        void(boost::system::error_code,
             boost::string_view)>::value and
        std::is_constructible<on_error_type, Handler, Allocator>::value, int>::type)
{
    on_error_ = on_error_type(std::move(handler), alloc);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Handler, class Allocator>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::member(
        typename option::on_timer_t, Handler& handler, const Allocator& alloc,
        typename std::enable_if<
        base::traits::TryInvoke<Handler,
        void(context_type)>::value and
        std::is_constructible<on_timer_type, Handler, Allocator>::value, int>::type)
{
    on_timer_ = on_timer_type(std::move(handler), alloc);
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
typename session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::socket_type&
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::member(
        typename option::get_socket_t)
{
    return connection_.asio_socket();
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
        do_eof(shutdown_type::shutdown_both);
        return;
    }

    if (ec) {
        if (on_error_)
            on_error_(ec, "async_read/on_read");

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

        return;
    }

    if (close) {
        do_eof(shutdown_type::shutdown_both);
        return;
    }

    if (queue_.on_write())
        recv();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_write(
        response_type<_Body>& response)
{
    connection_.async_write(
                response,
                std::bind(&flesh::on_write, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2,
                          response.need_eof()));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _Body>
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_push(
        response_type<_Body>& response)
{
    auto ec = connection_.write(response);

    if (ec) {
        if (on_error_)
            on_error_(ec, "write/do_push");

        return;
    }
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_read()
{
    connection_.async_read(
                buffer_,
                *parser_,
                std::bind(&flesh::on_read, this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2));
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
void
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::flesh::do_eof(shutdown_type type)
{
    if (not connection_.stream().is_open())
        return;

    auto ec = connection_.shutdown(type);
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
    request_type request = parser_->release();

    {
        BEASTHTTP_LOCKABLE_ENTER_TO_READ(*router_mutex_)
        this->provide(request, *this);
    }

    if (not queue_.is_full() and connection_.stream().is_open())
        recv();
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
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::make(socket_type&& socket,
     Router const& router, buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::make(allocator_t arg,
     socket_type&& socket, Router const& router, buffer_type&& buffer,
     const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::make(socket_type&& socket,
     Router const& router, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer), std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::make(allocator_t arg,
     socket_type&& socket, Router const& router, const Deleter& d,
     const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(socket_type&& socket,
     Router const& router, buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.recv();

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(allocator_t arg, socket_type&& socket,
     Router const& router, buffer_type&& buffer, const Deleter& d, const Allocator& alloc,
     _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.recv();

    return ctx;
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

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.recv();

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(allocator_t arg,
     socket_type&& socket, Router const& router, const Deleter& d,
     const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.recv();

    return ctx;
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
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.recv(timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(allocator_t arg,
     socket_type&& socket, Router const& router, TimePointOrDuration const timeOrDuration,
     buffer_type&& buffer, const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).recv(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.recv(timeOrDuration);

    return ctx;
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

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.recv(timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::recv(allocator_t arg,
     socket_type&& socket, Router const& router, TimePointOrDuration const timeOrDuration,
     const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).recv(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.recv(timeOrDuration);

    return ctx;
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
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.send(std::forward<Response>(response));

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class Response, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(allocator_t arg,
     socket_type&& socket, Response&& response, Router const& router, buffer_type&& buffer,
     const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.send(std::forward<Response>(response));

    return ctx;
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

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.send(std::forward<Response>(response));

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class Response, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(allocator_t arg,
     socket_type&& socket, Response&& response, Router const& router, const Deleter& d,
     const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.send(std::forward<Response>(response));

    return ctx;
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
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.send(std::forward<Response>(response), timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class Response, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(allocator_t arg,
     socket_type&& socket, Response&& response, Router const& router, TimePointOrDuration const timeOrDuration,
     buffer_type&& buffer, const Deleter& d, const Allocator& alloc,  _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>(), std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.send(std::forward<Response>(response), timeOrDuration);

    return ctx;
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

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.send(std::forward<Response>(response), timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class Response, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::send(allocator_t arg,
     socket_type&& socket, Response&& response, Router const& router, TimePointOrDuration const timeOrDuration,
     const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).send(std::declval<Response>(), std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.send(std::forward<Response>(response), timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(socket_type&& socket,
     Router const& router, buffer_type&& buffer, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.wait();

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(allocator_t arg,
     socket_type&& socket, Router const& router, buffer_type&& buffer, const Deleter& d,
     const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.wait();

    return ctx;
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

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.wait();

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(allocator_t arg,
     socket_type&& socket, Router const& router, const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(std::declval<Router const&>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.wait();

    return ctx;
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
#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.wait(timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(allocator_t arg,
     socket_type&& socket, Router const& router, TimePointOrDuration const timeOrDuration, buffer_type&& buffer,
     const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).wait(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.wait(timeOrDuration);

    return ctx;
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

#if not defined BEASTHTTP_USE_MAKE_SHARED
    using Alloc = std::allocator<flesh_type>;

    Alloc a = Alloc();

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...)));
#else
    context_type ctx(*std::make_shared<flesh_type>(
                         std::move(socket), router.resource_map(), router.method_map(),
                         router.regex_flags(), &router.mutex(), std::move(buffer),
                         std::forward<_OnAction>(on_action)...));
#endif // BEASTHTTP_USE_MAKE_SHARED

    ctx.wait(timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class Deleter, class Allocator, class Router, class TimePointOrDuration, class... _OnAction>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::wait(allocator_t arg,
     socket_type&& socket, Router const& router, TimePointOrDuration const timeOrDuration,
     const Deleter& d, const Allocator& alloc, _OnAction&&... on_action) -> typename std::decay<decltype (
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE(
            std::declval<Router const&>()).wait(std::declval<TimePointOrDuration>()),
        std::declval<context_type>())>::type
{
    buffer_type buffer;

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<flesh_type>;

    Alloc a = Alloc(alloc);

    context_type ctx(*std::shared_ptr<flesh_type>(
                         new (std::allocator_traits<Alloc>::allocate(a, 1)) flesh_type(
                             std::move(socket), router.resource_map(), router.method_map(),
                             router.regex_flags(), &router.mutex(), std::move(buffer),
                             std::forward<_OnAction>(on_action)...), d, alloc));

    ctx.wait(timeOrDuration);

    return ctx;
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _OnError>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::eof(socket_type&& socket,
    _OnError&& on_error) -> decltype (void(
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY()))
{
    buffer_type buffer;
    return flesh_type(std::move(socket), {}, {}, {}, {}, std::move(buffer),
                      std::forward<_OnError>(on_error)).eof();
}

BEASTHTTP_REACTOR_SESSION_TMPL_DECLARE
template<class _OnError>
auto
session<BEASTHTTP_REACTOR_SESSION_TMPL_ATTRIBUTES>::cls(socket_type&& socket,
    _OnError&& on_error) -> decltype (void(
        BEASTHTTP_REACTOR_SESSION_TRY_INVOKE_FLESH_TYPE_LEGACY()))
{
    buffer_type buffer;
    return flesh_type(std::move(socket), {}, {}, {}, {}, std::move(buffer),
                      std::forward<_OnError>(on_error)).cls();
}

} // namespace reactor
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_REACTOR_IMPL_SESSION_HXX
