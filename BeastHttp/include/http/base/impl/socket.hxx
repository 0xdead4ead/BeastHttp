#if not defined BEASTHTTP_BASE_IMPL_SOCKET_HXX
#define BEASTHTTP_BASE_IMPL_SOCKET_HXX

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Socket>
boost::system::error_code
socket<Socket>::shutdown(typename socket_type::shutdown_type type)
{
    auto ec = boost::system::error_code{};
    instance_.shutdown(type, ec);

    return ec;
}

template<class Socket>
boost::system::error_code
socket<Socket>::close()
{
    auto ec = boost::system::error_code{};
    instance_.close(ec);

    return ec;
}

template<class Socket>
socket<Socket>::socket(socket_type&& socket)
    : instance_{std::move(socket)}
{
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_SOCKET_HXX
