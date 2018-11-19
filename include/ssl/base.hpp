#ifndef BEAST_HTTP_SSL_BASE_HPP
#define BEAST_HTTP_SSL_BASE_HPP

#include <boost/asio/ssl/stream.hpp>

#include "../base.hpp"

namespace http {

namespace ssl {

namespace base {

/// \brief The ssl connection class
class connection : public http::base::connection_base<connection> {

    using base_t = http::base::connection_base<connection>;

    boost::asio::ip::tcp::socket socket_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> stream_;

public:

    using ptr = std::shared_ptr<connection>;

    ~connection() = default;

    // Constructor for server to client connection
    explicit connection(boost::asio::ssl::context & ctx,
                        boost::asio::ip::tcp::socket&& socket)
        : base_t{socket.get_executor()},
          socket_{std::move(socket)},
          stream_{socket_, ctx}
    {}

    // Constructor for client to server connection
    explicit connection(boost::asio::ssl::context & ctx,
                        boost::asio::io_service& ios)
        : base_t{ios.get_executor()},
          socket_{ios},
          stream_{socket_, ctx}
    {}

    template<class F>
    auto async_connect(const boost::asio::ip::tcp::endpoint& endpoint, F&& f){
        return socket_.async_connect(endpoint, std::forward<F>(f));
    }

    auto connect(const boost::asio::ip::tcp::endpoint& endpoint){
        auto ec = boost::system::error_code{};

        socket_.connect(endpoint, ec);

        if(ec)
            http::base::fail(ec, "connect");

        return ec;
    }

    template <class F, class B>
    auto async_handshake_server(const B& buf, F&& f){
        return stream_.async_handshake(
            boost::asio::ssl::stream_base::server, buf.data(),
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    template <class F>
    auto async_handshake_client(F&& f){
        return stream_.async_handshake(
            boost::asio::ssl::stream_base::client,
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    template<class B>
    auto handshake_server(const B& buf){
        auto ec = boost::beast::error_code{};
        stream_.handshake(
                    boost::asio::ssl::stream_base::server, buf.data(), ec);

        if(ec)
            http::base::fail(ec, "handshake");

        return ec;
    }

    auto handshake_client(){
        auto ec = boost::beast::error_code{};
        stream_.handshake(boost::asio::ssl::stream_base::client, ec);

        if(ec)
            http::base::fail(ec, "handshake");

        return ec;
    }

    template<class F>
    auto async_shutdown(F&& f){
        return stream_.async_shutdown(
                    boost::asio::bind_executor(
                        strand_, std::forward<F>(f)));
    }

    auto shutdown() {
        auto ec = boost::beast::error_code{};
        stream_.shutdown(ec);

        if(ec)
            http::base::fail(ec, "shutdown");

        return ec;
    }

    auto & stream(){
        return stream_;
    }

    auto release_stream(){
        return std::move(socket_);
    }

};

} // namespace base

// synchronous handshake calls
auto handshake_server(base::connection & connection){
    boost::beast::flat_buffer buffer;
    return connection.handshake_server(buffer);
}

auto handshake_client(base::connection & connection){
    return connection.handshake_client();
}
////

class RAIIConnection : public RAII,
        public base::connection{

    bool is_valid_ = false;

public:

    RAIIConnection(const boost::asio::ip::tcp::endpoint& endpoint,
                   boost::asio::ssl::context & ctx)
        : base::connection{ctx, http::base::processor::get().io_service()}
    {
        auto ec = connect(endpoint);

        if(!ec)
            is_valid_ = true;
    }

    virtual ~RAIIConnection() override;

    virtual operator bool() override{
        return is_valid_;
    }

}; // class RAIIConnection

RAIIConnection::~RAIIConnection(){
    shutdown();
}

} // namespace ssl

} // namespace http

#endif // BEAST_HTTP_SSL_BASE_HPP
