#if not defined BEASTHTTP_BASE_SOCKET_HXX
#define BEASTHTTP_BASE_SOCKET_HXX

#include <type_traits>

#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>

#define BEASTHTTP_SOCKET_TMPL_ATTRIBUTES \
    Protocol, Socket

namespace http {
namespace base {

template<class Protocol,
         template<typename> class Socket>
class socket
{
    using self_type = socket;

protected:

    using protocol_type = Protocol;

    using Sock = Socket<protocol_type>;

    static_assert (std::is_base_of<boost::asio::socket_base, Sock>::value,
                   "Socket type is not supported!");

    socket(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    boost::system::error_code
    shutdown(typename Sock::shutdown_type type)
    {
        auto ec = boost::system::error_code{};
        instance_.shutdown(type, ec);

        return ec;
    }

    boost::system::error_code
    close()
    {
        auto ec = boost::system::error_code{};
        instance_.close(ec);

        return ec;
    }

    Sock
    release()
    {
        return std::move(instance_);
    }

    explicit
    socket(Sock&& sock)
        : instance_{std::move(sock)}
    {}

    Sock instance_;

}; // class socket

} // namespace base
} // namespace http

#endif // BEASTHTTP_BASE_SOCKET_HXX
