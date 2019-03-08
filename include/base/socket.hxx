#if not defined BEASTHTTP_BASE_SOCKET_HXX
#define BEASTHTTP_BASE_SOCKET_HXX

#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>

#define BEASTHTTP_SOCKET_TMPL_ATTRIBUTES \
    Protocol, Socket

namespace _0xdead4ead {
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
    shutdown(typename Sock::shutdown_type);

    boost::system::error_code
    close();

    Sock
    release();

    explicit
    socket(Sock&& sock);

    Sock instance_;

}; // class socket

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/socket.ixx>

#endif // BEASTHTTP_BASE_SOCKET_HXX
