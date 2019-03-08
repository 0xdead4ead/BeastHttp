#if not defined BEASTHTTP_BASE_SOCKET_IXX
#define BEASTHTTP_BASE_SOCKET_IXX

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Protocol,
         template<typename> class Socket>
boost::system::error_code
socket<Protocol, Socket>::shutdown(typename Sock::shutdown_type type)
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
typename socket<Protocol, Socket>::Sock
socket<Protocol, Socket>::release()
{
    return std::move(instance_);
}

template<class Protocol,
         template<typename> class Socket>
socket<Protocol, Socket>::socket(Sock&& sock)
    : instance_{std::move(sock)}
{
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_SOCKET_IXX
