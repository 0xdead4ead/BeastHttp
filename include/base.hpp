#ifndef BEAST_HTTP_BASE_HPP
#define BEAST_HTTP_BASE_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/windows/stream_handle.hpp>
#include <boost/config.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/signal_set.hpp>
#include <fstream>
#include <streambuf>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <memory>

#define BEAST_HTTP_VERSION 106

namespace http {

namespace base {

class processor;

void out(const boost::beast::string_view & info);
void fail(const boost::system::error_code & ec,
          const boost::beast::string_view & info);

class timer{

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer_;

public:

    using ptr = std::shared_ptr<timer>;
    using duration = boost::asio::steady_timer::duration;
    using time_point = boost::asio::steady_timer::time_point;

    template<class Time>
    explicit timer(boost::asio::io_context::executor_type executor,
                   const Time& duration_or_time)
        : strand_{executor},
          timer_{executor.context(), duration_or_time}
    {}

    auto & stream(){
        return timer_;
    }

    template<class F>
    auto async_wait(F&& f){
        return timer_.async_wait(
                    boost::asio::bind_executor(
                        strand_, std::forward<F>(f)));
    }

}; // class timer

//###########################################################################

/// \brief The connection class
template<class Derived>
class connection_base{

    Derived& derived(){
        return static_cast<Derived&>(*this);
    }

protected:

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;

public:

    explicit connection_base(boost::asio::io_context::executor_type executor)
        : strand_{executor}
    {}

    template <class F, class R>
    auto async_write(/*const*/ R& msg, F&& f){
        return boost::beast::http::async_write(derived().stream(), msg,
                                   boost::asio::bind_executor(
                                           strand_, std::forward<F>(f)));
    }

    template <class F, class B, class R>
    auto async_read(B& buf, R& msg, F&& f){
        return boost::beast::http::async_read(derived().stream(), buf, msg,
                                   boost::asio::bind_executor(
                                           strand_, std::forward<F>(f)));
    }

    template<class R, class B>
    auto read(B& buf, R& msg){
        auto ec = boost::beast::error_code{};

        boost::beast::http::read(derived().stream(), buf, msg, ec);

        if(ec)
            fail(ec, "read");

        return ec;
    }

    template<class R>
    auto write(R& msg){
        auto ec = boost::beast::error_code{};

        boost::beast::http::write(derived().stream(), msg, ec);

        if(ec)
            fail(ec, "write");

        return ec;
    }

}; // connection class

/// \brief The plain connection class
class connection : public connection_base<connection> {

    using base_t = connection_base<connection>;

    boost::asio::ip::tcp::socket socket_;

public:

    using ptr = std::shared_ptr<connection>;

    connection(connection&&) = default;
    auto operator=(connection&&) -> connection& = default;
    ~connection() = default;

    // Constructor for server to client connection
    explicit connection(boost::asio::ip::tcp::socket&& socket)
        : base_t{socket.get_executor()},
          socket_{std::move(socket)}
    {}

    // Constructor for client to server connection
    explicit connection(boost::asio::io_service& ios)
        : base_t{ios.get_executor()},
          socket_{ios}
    {}

    template<class F>
    auto async_connect(const boost::asio::ip::tcp::endpoint& endpoint, F&& f){
        return socket_.async_connect(endpoint, std::forward<F>(f));
    }

    auto connect(const boost::asio::ip::tcp::endpoint& endpoint){
        auto ec = boost::system::error_code{};

        socket_.connect(endpoint, ec);

        if(ec)
            fail(ec, "connect");

        return ec;
    }

    auto shutdown(boost::asio::ip::tcp::socket::shutdown_type type) {
        auto ec = boost::beast::error_code{};
        socket_.shutdown(type, ec);

        if(ec)
            fail(ec, "shutdown");

        return ec;
    }

    auto close(){
        auto ec = boost::beast::error_code{};
        socket_.close(ec);

        if(ec)
            fail(ec, "close");

        return ec;
    }

    auto & stream(){
        return socket_;
    }

    auto release_stream(){
        return std::move(socket_);
    }

}; // plain_connection class

//###########################################################################

/// \brief The listener class
class listener : public std::enable_shared_from_this<listener>
{

    using acceptor_type = boost::asio::ip::tcp::acceptor;
    using on_accept_fn = std::function<void (boost::asio::ip::tcp::socket&&)>;

public:

    using ptr = std::shared_ptr<listener>;

    listener(listener&&) = default;
    auto operator=(listener&&) -> listener& = default;

    explicit listener(boost::asio::io_service& io_service)
        : acceptor_{io_service},
          socket_{io_service}
    {}

    auto init(const boost::asio::ip::tcp::endpoint& endpoint){

        auto ec = boost::system::error_code{};
        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            return ec;
        }

        // Allow address reuse
        acceptor_.set_option(boost::asio::socket_base::reuse_address(false));
        if(ec)
        {
            fail(ec, "set_option");
            return ec;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            return ec;
        }

        // Start listening for connections
        acceptor_.listen(
            boost::asio::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            return ec;
        }

        endpoint_ = endpoint;

        out(std::string("Start listening on ")
            + endpoint.address().to_string()
            + ":" + std::to_string(endpoint.port()));

        return ec;
    }

    auto close() {
        auto ec = boost::system::error_code{};
        acceptor_.close(ec);

        if(ec)
            fail(ec, "close");

        return ec;
    }

    template<class F>
    auto do_accept(F&& on_accept){
        return acceptor_.async_accept(
                    socket_, std::forward<F>(on_accept));
    }

    // Start accepting incoming connections
    template<class F>
    void run(F&& on_accept){
        if(!acceptor_.is_open()){
            out("Acceptor could not open!");
            return;
        }

        do_accept(std::forward<F>(on_accept));
    }

    auto & endpoint() const{
        return endpoint_;
    }

    auto get_socket(){
        return std::move(socket_);
    }

protected:

    acceptor_type acceptor_;
    boost::asio::ip::tcp::socket socket_;

private:

    boost::asio::ip::tcp::endpoint endpoint_;

}; // class tcp_listener

//###########################################################################

/// \brief The base_processor class
class processor : private boost::noncopyable {

private:

    using listeners_ptr_map = std::map<boost::asio::ip::tcp::endpoint,
                                                                 listener::ptr>;

public:

    static auto& get(){
        static processor base_proc;
        return base_proc;
    }

    void start(std::size_t threads_count) {
        for (std::size_t i = 0; i < threads_count; ++i)
            threads_pool_.create_thread(
               std::bind(static_cast<std::size_t (boost::asio::io_service::*)()>
                          (&boost::asio::io_service::run), std::ref(ios_)));
    }

    void stop() {
        ios_.stop();
    }

    void wait(){
        threads_pool_.join_all();
    }

    template<class Listener,
             class = std::enable_if_t<
                 std::is_same<
                     std::decay_t<Listener>, listener>::value
                 >
             >
    auto & add_listener(Listener&& listener) {
        auto& endpoint = listener.endpoint();

        listeners_ptr_map::const_iterator it = listeners_.find(endpoint);
        if (it != listeners_.cend()) {
            out("Endpoint " + endpoint.address().to_string()
                + ":" + std::to_string(endpoint.port())
                + " is already created!");
        }

        listeners_.insert({
                              endpoint,
                              std::make_shared<Listener>
                                     (std::move(listener))
                          });

        return *listeners_.at(endpoint);
    }

    void remove_listener(const boost::asio::ip::tcp::endpoint& endpoint) {
        listeners_ptr_map::const_iterator it = listeners_.find(endpoint);
        if (it == listeners_.cend()) {
            out("No listener for " + endpoint.address().to_string()
                + ":" + std::to_string(endpoint.port()));
            return;
        }

        listeners_.erase(it);
        out("Listener " + endpoint.address().to_string()
            + ":" + std::to_string(endpoint.port()) + " is removed!");
    }

    boost::asio::ip::tcp::endpoint resolve(const std::string & address,
                                           uint32_t port,
                                           boost::system::error_code& ec){
        auto resolver = boost::asio::ip::tcp::resolver{ios_};
        auto query = boost::asio::ip::tcp::resolver::query
                {address, boost::lexical_cast<std::string>(port)};

        auto resolved = resolver.resolve(query, ec);
        if(ec){
            fail(ec, "resolve");
            return {};
        }

        return *resolved;
    }

    // This function is not threads safe!
    // Must be called before all the `start()` calls
    // Function can be called only once
    template <class F>
    void register_signals_handler(F&& f,
                                  const std::vector<int>& signals_to_wait){
        // Making shure that this is the first call
        assert(!signal_handlers_);

        signal_handlers_ = std::forward<F>(f);
        std::for_each(
            signals_to_wait.begin(),
            signals_to_wait.end(),
            boost::bind(
                &boost::asio::signal_set::add, &signals_, _1
            )
        );

        signals_.async_wait(boost::bind(
            &processor::handle_signals, this, _1, _2
        ));
    }

#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
    using native_handle_type
                    = boost::asio::posix::stream_descriptor::native_handle_type;
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
    using native_handle_type
                      = boost::asio::windows::stream_handle::native_handle_type;
#else
#error "Neither BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR"
    " nor BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE is defined"
#endif

    void assign_in(const native_handle_type & handle){
        in_.assign(handle);
    }

    void assign_out(const native_handle_type & handle){
        out_.assign(handle);
    }

    template<class Completion>
    std::size_t read_from_stream(std::string & value, Completion && completion){
        if(in_.is_open())
            return boost::asio::read(in_, boost::asio::buffer(value),
                                     std::forward<Completion>(completion));

        return 0;
    }

    template<class Completion, class Callback>
    void async_read_from_stream(std::string & value, Completion && completion,
                                Callback && callback){
        if(in_.is_open())
            boost::asio::async_read(in_, boost::asio::buffer(value),
                                    std::forward<Completion>(completion),
                                    std::forward<Callback>(callback));
    }

    template<class Completion>
    std::size_t write_to_stream(const std::string & value,
                                Completion && completion){
        if(out_.is_open())
            return boost::asio::write(out_, boost::asio::buffer(value),
                                      std::forward<Completion>(completion));

        return 0;
    }

    template<class Completion, class Callback>
    void async_write_to_stream(const std::string & value,
                               Completion && completion, Callback && callback){
        if(out_.is_open())
            boost::asio::async_write(out_, boost::asio::buffer(value),
                                     std::forward<Completion>(completion),
                                     std::forward<Callback>(callback));
    }

    auto & io_service(){
        return ios_;
    }

private:

    boost::asio::io_service ios_;
    boost::asio::io_service::work   work_;
    boost::thread_group threads_pool_;
    listeners_ptr_map listeners_;
    boost::asio::signal_set signals_;
#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
    boost::asio::posix::stream_descriptor in_;
    boost::asio::posix::stream_descriptor out_;
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
    boost::asio::windows::stream_handle in;
    boost::asio::windows::stream_handle out;
#endif

    std::function<void(int)> signal_handlers_;

    processor()
        : ios_{}
        , work_{ios_},
          signals_{ios_},
#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
          in_{ios_, ::dup(STDIN_FILENO)},
          out_{ios_, ::dup(STDOUT_FILENO)}
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
          in_{ ios_ },
          out_{ ios_ }
#endif
    {}

    void handle_signals(const boost::system::error_code& error,
                        int signal_number){
        if (error)
            fail(error, "handle_signals");
        else
            signal_handlers_(signal_number);


        signals_.async_wait
                (boost::bind(&processor::handle_signals, this, _1, _2));
    }

}; // class processor

inline void read_all(std::string & value){
    http::base::processor::get()
            .read_from_stream(value, boost::asio::transfer_all());
}

inline void read_up_to_enter(std::string & value){
    http::base::processor::get()
            .read_from_stream(value,
                              boost::bind<size_t>([](auto & value, auto & err)
                                                  -> size_t{
        if ( err)
            return 0;

        if(std::find(value.cbegin(), value.cend(), '\n') != value.cend())
            return 0;

        return 1;
    }, boost::ref(value), _1));
}

std::string prefix_line(){
    boost::posix_time::ptime timeLocal
                                = boost::posix_time::second_clock::local_time();

    std::ostringstream os;
    os << '(' << "BeastHttp/"
       << BEAST_HTTP_VERSION << " [" << BOOST_BEAST_VERSION_STRING << ']' << ' '
       << timeLocal.date().year() << '/' << timeLocal.date().day() << '/'
       << timeLocal.date().month() << ' '
       << timeLocal.time_of_day().hours()
       << ':' << timeLocal.time_of_day().minutes()
       << ':' << timeLocal.time_of_day().seconds() << ')' << ' ';
    return os.str();
}

inline void out(const boost::beast::string_view & info){
    const std::string & out_line = prefix_line() + info.to_string() + "\n";
    processor::get().write_to_stream(out_line,
                                boost::asio::transfer_exactly(out_line.size()));
}

inline void fail(const boost::system::error_code & ec,
                 const boost::beast::string_view & info){
    const std::string & out_line = prefix_line()
            + info.to_string() + " : " + ec.message() + "\n";
    processor::get().write_to_stream(out_line,
                                boost::asio::transfer_exactly(out_line.size()));
}

} // namespace base

class RAII;

// synchronous calls
template<class Connection, class Body,
         class = std::enable_if_t<std::is_base_of<base::connection_base<Connection>, Connection>::value
                                  || std::is_base_of<RAII, Connection>::value>>
auto send(Connection & connection,
                  /*const*/ boost::beast::http::request<Body> & msg){
    return connection.write(msg);
}

template<class Connection, class Body,
         class = std::enable_if_t<std::is_base_of<base::connection_base<Connection>, Connection>::value
                                  || std::is_base_of<RAII, Connection>::value>>
auto send(Connection & connection,
                   /*const*/ boost::beast::http::response<Body> & msg){
    return connection.write(msg);
}

template<class Connection, class Body,
         class = std::enable_if_t<std::is_base_of<base::connection_base<Connection>, Connection>::value
                                  || std::is_base_of<RAII, Connection>::value>>
auto recv(Connection & connection,
                  boost::beast::http::request<Body> & msg){
    boost::beast::flat_buffer buffer;
    return connection.read(buffer, msg);
}

template<class Connection, class Body,
         class = std::enable_if_t<std::is_base_of<base::connection_base<Connection>, Connection>::value
                                  || std::is_base_of<RAII, Connection>::value>>
auto recv(Connection & connection,
                   boost::beast::http::response<Body> & msg){
    boost::beast::flat_buffer buffer;
    return connection.read(buffer, msg);
}
////

class RAII{

public:

    virtual ~RAII();
    virtual operator bool() = 0;

};

RAII::~RAII(){}

class RAIIConnection : public RAII,
        public base::connection{

    bool is_valid_ = false;

public:

    RAIIConnection(const boost::asio::ip::tcp::endpoint& endpoint)
        : base::connection{base::processor::get().io_service()}
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
    close();
}

} // namespace http

#endif // BEAST_HTTP_BASE_HPP
