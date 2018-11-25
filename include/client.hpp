#ifndef BEAST_HTTP_CLIENT_HPP
#define BEAST_HTTP_CLIENT_HPP

#include "base.hpp"
#include "session.hpp"

namespace http{

/// @brief Class for communication with a remote host
/// @tparam Body type for response message
template<class ResBody>
class client_impl{

    using self_type = client_impl<ResBody>;
    using session_type = session<false, ResBody>;

    using on_connection_fn
            = std::function<void (session_type&)>;

    using on_message_fn
            = std::function<void (boost::beast::http::response<ResBody>&,
                                  session_type&)>;
    using on_error_fn
            = std::function<void (boost::beast::error_code const &,
                                  boost::beast::string_view const&)>;

    void on_connect_(const boost::system::error_code & ec){
        if(ec){
            base::fail(ec, "connect");

            if(on_error)
                on_error(ec, "connect");

            return;
        }

        session<false, ResBody>::
                on_connect(connection_, on_connect, on_message, on_error);
    }

    bool process(std::string const & host, uint32_t port){
        auto ec = boost::system::error_code{};
        auto endpoint = processor_.resolve(host, port, ec);

        if(ec){
            if(on_error)
                on_error(ec, "resolve");

            return false;
        }

        connection_.async_connect(endpoint,
                                  std::bind(&self_type::on_connect_, this, std::placeholders::_1));

        return true;
    }

    base::processor& processor_;
    base::connection connection_;

public:

    on_connection_fn on_connect;
    on_message_fn on_message;
    on_error_fn on_error;

    explicit client_impl()
        :  processor_{base::processor::get()},
          connection_{processor_.io_service()}
    {}

//    template<class Context>
//    explicit client_impl(Context& ctx)
//        :  processor_{base::processor::get()},
//          connection_{processor_.create<base::connection>(ctx)}
//    {}

    bool invoke(std::string const & host, uint32_t port){
        return process(host, port);
    }

    template<class Callback1, class Callback2, class Callback3>
    bool invoke(std::string const & host, uint32_t port,
                Callback1&& on_connect_handler, Callback2&& on_message_handler, Callback3&& on_error_handler){
        on_connect = std::forward<Callback1>(on_connect_handler);
        on_message = std::forward<Callback2>(on_message_handler);
        on_error = std::forward<Callback3>(on_error_handler);
        return process(host, port);
    }

}; // client_impl class

using client = client_impl<boost::beast::http::string_body>;

} // namespace http

#endif // BEAST_HTTP_CLIENT_HPP
