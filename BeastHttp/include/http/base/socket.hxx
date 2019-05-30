#if not defined BEASTHTTP_BASE_SOCKET_HXX
#define BEASTHTTP_BASE_SOCKET_HXX

#include <boost/asio/socket_base.hpp>
#include <boost/system/error_code.hpp>

#define BEASTHTTP_SOCKET_TMPL_ATTRIBUTES \
    Socket

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Socket>
class socket
{
    using self_type = socket;

protected:

    using protocol_type = typename Socket::protocol_type;

    using socket_type = Socket;

    static_assert (std::is_base_of<boost::asio::socket_base, socket_type>::value,
                   "Socket type is not supported!");

    socket(self_type&&) = default;
    auto operator=(self_type&&) -> self_type& = default;

    boost::system::error_code
    shutdown(typename socket_type::shutdown_type);

    boost::system::error_code
    close();

    explicit
    socket(socket_type&&);

    socket_type instance_;

}; // class socket

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <http/base/impl/socket.hxx>

#endif // BEASTHTTP_BASE_SOCKET_HXX
