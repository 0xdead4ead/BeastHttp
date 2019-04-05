#if not defined BEASTHTTP_SHARED_DETECTOR_HXX
#define BEASTHTTP_SHARED_DETECTOR_HXX

#include <base/traits.hxx>
#include <base/detector.hxx>

#include <boost/asio/ip/tcp.hpp>

#define BEASTHTTP_SHARED_DETECTOR_TMPL_ATTRIBUTES \
    OnDetect, OnError, Buffer, Protocol, Socket

namespace _0xdead4ead {
namespace http {
namespace shared {

template<template<typename> class OnDetect = std::function,
         template<typename> class OnError = std::function,
         class Buffer = boost::beast::flat_buffer,
         class Protocol = boost::asio::ip::tcp,
         template<typename> class Socket = boost::asio::basic_stream_socket>
class detector : public std::enable_shared_from_this<detector<BEASTHTTP_SHARED_DETECTOR_TMPL_ATTRIBUTES>>,
        private base::detector, boost::asio::coroutine
{
    using self_type = detector;

    using base_type = base::detector;

public:

    using protocol_type = Protocol;

    using socket_type = Socket<protocol_type>;

    using buffer_type = Buffer;

    using on_detect_type = OnDetect<void (socket_type, buffer_type, boost::tribool)>;

    using on_error_type = OnError<void (boost::system::error_code, boost::string_view)>;

    template<class... _OnAction>
    static auto
    async(socket_type, _OnAction&&...) -> decltype (
            self_type(std::declval<socket_type>(), std::declval<_OnAction>()...), void());

    static boost::system::error_code
    sync(socket_type&, buffer_type&, boost::tribool&);

    template<class _OnDetect>
    explicit
    detector(socket_type, _OnDetect&&,
             typename std::enable_if<base::traits::TryInvoke<
             _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value, int>::type = 0);

    template<class _OnDetect, class _OnError>
    explicit
    detector(socket_type, _OnDetect&&, _OnError&&,
             typename std::enable_if<base::traits::TryInvoke<
             _OnDetect, void(socket_type&&, buffer_type&&, boost::tribool)>::value and
             base::traits::TryInvoke<_OnError, void(
                 boost::system::error_code, boost::string_view)>::value, int>::type = 0);

private:

    void
    do_async(boost::system::error_code = {}, boost::tribool = {});

    socket_type socket_;
    buffer_type buffer_;
    on_detect_type on_detect_;
    on_error_type on_error_;

};

namespace _default {

using detector_type = shared::detector<>;

} // namespace _default
} // namespace shared
} // namespace http
} // namespace _0xdead4ead

#include <shared/impl/detector.hxx>

#endif // not defined BEASTHTTP_SHARED_DETECTOR_HXX
