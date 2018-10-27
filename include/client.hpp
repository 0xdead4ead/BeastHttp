#ifndef BEAST_HTTP_CLIENT_HPP
#define BEAST_HTTP_CLIENT_HPP

#include "base.hpp"
#include "session.hpp"

namespace http{

/// \brief Class for communication with a remote host
/// \tparam Body type for response message
template<class ResBody>
class client_impl{

    bool process(std::string const & host, uint32_t port){
        connection_p_ = base::processor::get()
                .create_connection<base::connection>(host,
                                                     port,
                                                     [this](const boost::system::error_code & ec){
            if(ec){
                if(on_error){
                    on_error(ec, "connect");
                    return;
                }
            }

            session<false, ResBody>::on_connect(connection_p_, on_connect, on_message, on_error);
        });

        if(!connection_p_)
            return false;

        return true;
    }

    base::connection::ptr connection_p_;

public:

    std::function<void (session<false, ResBody>&)> on_connect;
    std::function<void (boost::beast::http::response<ResBody>&, session<false, ResBody>&)> on_message;
    std::function<void (boost::beast::error_code const &, boost::beast::string_view const &)> on_error;

    explicit client_impl()
        :  connection_p_{nullptr}
    {}

    bool invoke(std::string const & host, uint32_t port){
        return process(host, port);
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace http

#endif // BEAST_HTTP_CLIENT_HPP
