#if not defined BEASTHTTP_BASE_IMPL_SOCKET_HXX
#define BEASTHTTP_BASE_IMPL_SOCKET_HXX

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Protocol,
         template<typename> class Socket>
boost::system::error_code
socket<Protocol, Socket>::shutdown(typename socket_type::shutdown_type type)
{
    auto ec = boost::system::error_code{};
    instance_.shutdown(type, ec);

    return ec;
}

template<class Protocol,
         template<typename> class Socket>
boost::system::error_code
socket<Protocol, Socket>::close()
{
    auto ec = boost::system::error_code{};
    instance_.close(ec);

    return ec;
}

template<class Protocol,
         template<typename> class Socket>
socket<Protocol, Socket>::socket(socket_type&& socket)
    : instance_{std::move(socket)}
{
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_SOCKET_HXX
