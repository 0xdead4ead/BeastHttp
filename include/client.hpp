#ifndef BEAST_HTTP_CLIENT_HPP
#define BEAST_HTTP_CLIENT_HPP

#include "base.hpp"
#include "session.hpp"

namespace http{

/// \brief Class for communication with a remote host
/// \tparam Body type for response message
template<class ResBody>
class client_impl{

    template<class Callback0>
    bool process(std::string const & host, uint32_t port, Callback0 && on_error_handler){
        connection_p_ = base::processor::get()
                .create_connection<base::connection>(host,
                                                     port,
                                                     [this, on_error = std::forward<Callback0>(on_error_handler)](const boost::system::error_code & ec){
            if(ec){
                base::fail(ec, "connect");
                on_error(ec);
                return;
            }

            session<false, ResBody>::on_connect(connection_p_, on_connect, on_message);
        });

        if(!connection_p_)
            return false;

        return true;
    }

    base::connection::ptr connection_p_;

public:

    std::function<void (session<false, ResBody>&)> on_connect;
    std::function<void (boost::beast::http::response<ResBody>&, session<false, ResBody>&)> on_message;

    explicit client_impl()
        :  connection_p_{nullptr}
    {}

    template<class Callback0>
    bool invoke(std::string const & host, uint32_t port, Callback0 && on_error_handler){
        return process(host, port, std::forward<Callback0>(on_error_handler));
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace http

#endif // BEAST_HTTP_CLIENT_HPP
