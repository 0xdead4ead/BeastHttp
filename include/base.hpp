#ifndef BASE_HPP
#define BASE_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/windows/stream_handle.hpp>
#include <boost/config.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/system/error_code.hpp>
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


namespace http {

namespace base {

class processor;

void out(const std::string & info);
void fail(const std::string & info);
void fail(const boost::system::error_code & ec, const std::string & info);

namespace detail{

/// \brief class storing a task
/// \tparam functor
template <class F>
struct task_wrapped {

private:

    F f_;

public:

    explicit task_wrapped(const F& f)
        : f_{f}
    {}

    void operator()() {
         f_();
    }

    static inline auto make(const F& f) {
        return task_wrapped<F>(f);
    }

}; // class task_wrapped

//###########################################################################

/// \brief Executes the stored procedure after the time expires
/// \tparam Procedure itself
template <class F>
struct timer_task: public task_wrapped<F> {

private:

    using base_type = task_wrapped<F>;
    using timer_ptr = std::shared_ptr<boost::asio::deadline_timer>;
    timer_ptr timer_;

public:

    template <class Time>
    explicit timer_task(boost::asio::io_service& ios, const Time& duration_or_time, const F& f)
        : base_type{f},
          timer_{std::make_shared<boost::asio::deadline_timer>(
                     boost::ref(ios), duration_or_time)}
    {}

    void launch() const {
        timer_->async_wait(*this);
    }

    void operator()(const boost::system::error_code& error) {
        if (!error)
            base_type::operator()();
        else
            fail(error, "timer_task");
    }

    template <class Time>
    static inline auto make(boost::asio::io_service& ios, const Time& duration_or_time, const F& f)
    {
        return timer_task<F>(ios, duration_or_time, f);
    }

}; // class timer_task

} // detail namespace

//###########################################################################

/// \brief The connection class
template<class Derived>
class connection_base : private boost::noncopyable {

    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

protected:

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;

public:

    explicit connection_base(boost::asio::io_context::executor_type executor)
        : strand_{executor}
    {}

    template <class F, class R>
    void async_write(R& msg, F&& f)
    {
        boost::beast::http::async_write(derived().stream(), msg,
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    template <class F, class B, class R>
    void async_read(B& buf, R& msg, F&& f)
    {
        boost::beast::http::async_read(derived().stream(), buf, msg,
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    void shutdown() {
        boost::system::error_code ec;
        derived().stream().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

        if(ec)
            fail(ec, "shutdown");

    }

}; // connection class

/// \brief The plain connection class
class connection : public connection_base<connection> {

    using base_t = connection_base<connection>;

    boost::asio::ip::tcp::socket socket_;

public:

    using ptr = std::shared_ptr<connection>;

    // Constructor for server to client connection
    explicit connection(boost::asio::ip::tcp::socket&& socket)
        : base_t{socket.get_executor()},
          socket_{std::move(socket)}
    {}

    // Constructor for client to server connection
    template<class F>
    explicit connection(
            boost::asio::io_service& ios,
            const boost::asio::ip::tcp::endpoint& endpoint, F&& f)
        : base_t{ios.get_executor()},
          socket_{ios}
    {
        socket_.async_connect(endpoint, std::forward<F>(f));
    }

    auto & stream(){
        return socket_;
    }

}; // plain_connection class

//###########################################################################

/// \brief The listener class
class listener : public std::enable_shared_from_this<listener> ,
        private boost::noncopyable
{

    using acceptor_type = boost::asio::ip::tcp::acceptor;

public:

    listener(boost::asio::io_service& io_service, const std::string & address, const std::string & port)
        : acceptor_{io_service},
          socket_{io_service}
    {
        init(io_service, address, port);
    }

    template <class F>
    listener(boost::asio::io_service& io_service, const std::string & address, const std::string & port, F&& callback)
        : acceptor_{io_service},
          socket_{io_service},
          on_accept_cb_{std::forward<F>(callback)}
    {
        init(io_service, address, port);
    }

    void init(boost::asio::io_service& io_service, const std::string & address, const std::string & port){
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(address, port);

        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query, ec);

        if(ec){
            fail(ec, "resolve");
            acceptor_.get_io_service().stop();
            return;
        }

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            fail(ec, "open");
            acceptor_.get_io_service().stop();
            return;
        }

        // Allow address reuse
        acceptor_.set_option(boost::asio::socket_base::reuse_address(false));
        if(ec)
        {
            fail(ec, "set_option");
            acceptor_.get_io_service().stop();
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            fail(ec, "bind");
            acceptor_.get_io_service().stop();
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            boost::asio::socket_base::max_listen_connections, ec);
        if(ec)
        {
            fail(ec, "listen");
            acceptor_.get_io_service().stop();
            return;
        }
    }

    void stop() {
        boost::system::error_code ec;
        acceptor_.close(ec);

        if(ec)
            fail(ec, "close");
    }

    // Start accepting incoming connections
    void run()
    {
        if(!acceptor_.is_open())
            return;
        do_accept();
    }

    void do_accept()
    {
        acceptor_.async_accept(
            socket_,
            std::bind(
                &listener::on_accept,
                shared_from_this(),
                std::placeholders::_1));
    }

    void on_accept(boost::system::error_code error)
    {
        if(error){
            fail(error, "accept");
        }
        else
            on_accept_cb_(std::move(socket_), {});

        // Accept another connection
        do_accept();
    }

protected:

    acceptor_type acceptor_;
    boost::asio::ip::tcp::socket socket_;

private:

    std::function< void(boost::asio::ip::tcp::socket&&, boost::beast::flat_buffer&&) > on_accept_cb_;

}; // class tcp_listener

//###########################################################################

/// \brief The base_processor class
class processor : private boost::noncopyable {

private:

    using listener_ptr = std::shared_ptr<listener>;
    using duration_type = boost::asio::deadline_timer::duration_type;
    using time_type = boost::asio::deadline_timer::time_type;
    using listeners_ptr_map = std::map<uint32_t, listener_ptr>;

public:

    static auto& get(){
        static processor base_proc;
        return base_proc;
    }

    template <class F>
    inline void push_task(F&& f) {
        ios_.post(detail::task_wrapped<F>::make(std::forward<F>(f)));
    }

    void start(std::size_t threads_count) {
        for (std::size_t i = 0; i < threads_count; ++i)
            threads_pool_.create_thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios_)));
    }

    void stop() {
        ios_.stop();
    }

    void wait(){
        threads_pool_.join_all();
    }

    template <class F>
    void run_after(duration_type duration, F&& f) {
        detail::timer_task<F>::make(ios_, duration, std::forward<F>(f)).launch();
    }

    template <class F>
    void run_at(time_type time, F&& f) {
        detail::timer_task<F>::make(ios_, time, std::forward<F>(f)).launch();
    }

    template <class F>
    const auto & add_listener(const std::string & address, uint32_t port, F&& f) {
        listeners_ptr_map::const_iterator it = listeners_.find(port);
        if (it != listeners_.end()) {
            throw std::runtime_error("Port '"+ boost::lexical_cast<std::string>(port) + "' already created");
        }

        listeners_.insert({
                              port,
                              std::make_shared<listener>(ios_,address, boost::lexical_cast<std::string>(port),std::forward<F>(f))
                          });

        return listeners_.at(port);
    }

    auto remove_listener(uint32_t port) {
        listeners_ptr_map::iterator it = listeners_.find(port);
        if (it == listeners_.end()) {
            throw std::runtime_error("No listener for port '"+ boost::lexical_cast<std::string>(port) + "' created");
        }

        auto listener = it->second;
        listeners_.erase(it);
        return listener;
    }

    template<class Connection, class F>
    std::shared_ptr<Connection> create_connection(const std::string & address, uint32_t port, F&& f) {
        boost::asio::ip::tcp::resolver resolver(ios_);
        boost::asio::ip::tcp::resolver::query query(address, boost::lexical_cast<std::string>(port));

        boost::system::error_code ec;

        auto resolved = resolver.resolve(query, ec);
        if(ec){
            fail(ec, "resolve");
            return {};
        }

        boost::asio::ip::tcp::endpoint endpoint = *resolved;
        return std::make_shared<Connection>(ios_, endpoint, std::forward<F>(f));
    }

    // This function is not threads safe!
    // Must be called before all the `start()` calls
    // Function can be called only once
    template <class F>
    void register_signals_handler(F&& f, const std::vector<int>& signals_to_wait)
    {
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
    using native_handle_type = boost::asio::posix::stream_descriptor::native_handle_type;
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
    using native_handle_type = boost::asio::windows::stream_handle::native_handle_type;
#else
#error "Neither BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR nor BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE is defined"
#endif

    void assign_in(const native_handle_type & handle){
        in.assign(handle);
    }

    void assign_out(const native_handle_type & handle){
        out.assign(handle);
    }

    template<class Completion>
    std::size_t read_from_stream(std::string & value, Completion && completion){
        if(in.is_open())
            return boost::asio::read(in, boost::asio::buffer(value), std::forward<Completion>(completion));

        return 0;
    }

    template<class Completion, class Callback>
    void async_read_from_stream(std::string & value, Completion && completion, Callback && callback){
        if(in.is_open())
            boost::asio::async_read(in, boost::asio::buffer(value),
                                    std::forward<Completion>(completion), std::forward<Callback>(callback));
    }

    template<class Completion>
    std::size_t write_to_stream(const std::string & value, Completion && completion){
        if(out.is_open())
            return boost::asio::write(out, boost::asio::buffer(value), std::forward<Completion>(completion));

        return 0;
    }

    template<class Completion, class Callback>
    void async_write_to_stream(const std::string & value, Completion && completion, Callback && callback){
        if(out.is_open())
            boost::asio::async_write(out, boost::asio::buffer(value),
                                     std::forward<Completion>(completion), std::forward<Callback>(callback));
    }

    boost::asio::io_service & io_service()
    {
        return ios_;
    }

private:

    boost::asio::io_service ios_;
    boost::asio::io_service::work   work_;
    boost::thread_group threads_pool_;
    listeners_ptr_map listeners_;
    boost::asio::signal_set signals_;
#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
    boost::asio::posix::stream_descriptor in;
    boost::asio::posix::stream_descriptor out;
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
          in{ios_, ::dup(STDIN_FILENO)},
          out{ios_, ::dup(STDOUT_FILENO)}
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
          in{ *ios_ },
          out{ *ios_ }
#endif
    {}

    void handle_signals(const boost::system::error_code& error,int signal_number)
    {
        if (error)
            fail(error, "handle_signals");
        else
            signal_handlers_(signal_number);


        signals_.async_wait(boost::bind(&processor::handle_signals, this, _1, _2));
    }

}; // class processor

void read_all(std::string & value){
    http::base::processor::get().read_from_stream(value, boost::asio::transfer_all());
}

void read_up_to_enter(std::string & value){
    http::base::processor::get()
            .read_from_stream(value, boost::bind<size_t>([](auto & value, auto & err) -> size_t{
        if ( err)
            return 0;

        if(std::find(value.cbegin(), value.cend(), '\n') != value.cend())
            return 0;

        return 1;
    }, boost::ref(value), _1));
}

void out(const std::string & info){
    std::ostringstream os;
    os << info << std::endl;
    const std::string & info_ = os.str();
    processor::get().write_to_stream(info_, boost::asio::transfer_exactly(info_.size()));
}

void fail(const std::string & info){
    std::ostringstream os;
    os << info << std::endl;
    const std::string & info_ = os.str();
    processor::get().write_to_stream(info_, boost::asio::transfer_exactly(info_.size()));
}

void fail(const boost::system::error_code & ec, const std::string & info){
    std::ostringstream os;
    os << info  << " : " << ec.message() << std::endl;
    const std::string & info_ = os.str();
    processor::get().write_to_stream(info_, boost::asio::transfer_exactly(info_.size()));
}

} // namespace base

} // namespace http

#endif // BASE_HPP
