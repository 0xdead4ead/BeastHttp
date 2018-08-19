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

/// \brief The tcp_connection class
class tcp_connection : private boost::noncopyable {

public:

    using ptr = std::shared_ptr<tcp_connection>;

    explicit tcp_connection(boost::asio::io_service& ios)
        : socket_{ios}, strand_{socket_.get_executor()}
    {}

    template<class F>
    explicit tcp_connection(
            boost::asio::io_service& ios,
            const boost::asio::ip::tcp::endpoint& endpoint, F&& f)
        : socket_{ios}, strand_{socket_.get_executor()}
    {
        socket_.async_connect(endpoint, std::forward<F>(f));
    }

    template <class F, class R>
    void async_write(R& msg, F&& f)
    {
        boost::beast::http::async_write(socket_, msg,
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    template <class F, class B, class R>
    void async_read(B& buf, R& msg, F&& f)
    {
        boost::beast::http::async_read(socket_, buf, msg,
            boost::asio::bind_executor(
                strand_, std::forward<F>(f)));
    }

    void shutdown() {
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

        if(ec)
            fail(ec, "shutdown");

    }

    auto & socket(){
        return socket_;
    }

private:

    boost::asio::ip::tcp::socket socket_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;

}; // tcp_connection class

//###########################################################################

/// \brief The tcp_listener class
class tcp_listener: public std::enable_shared_from_this<tcp_listener> ,
        private boost::noncopyable
{

    using acceptor_type = boost::asio::ip::tcp::acceptor;

public:

    template <class F>
    tcp_listener(boost::asio::io_service& io_service, const std::string & address, const std::string & port, F&& callback)
        : acceptor_{io_service}
        , f_callback_{std::forward<F>(callback)}
    {
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
        acceptor_.close();
    }

    void accept_start() {
        if (!acceptor_.is_open()) {
            return;
        }

        auto new_connection = std::make_shared<tcp_connection>(acceptor_.get_io_service());
        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(
                                   &tcp_listener::handle_accept,
                                   shared_from_this(),
                                   new_connection,
                                   boost::asio::placeholders::error
                                   ));
    }

private:

    void handle_accept(const tcp_connection::ptr & new_connection, const boost::system::error_code& error)
    {
        if (!error){
            accept_start();
            f_callback_(new_connection); // running task
        }else{
            fail(error, "accept");
            acceptor_.get_io_service().stop();
        }
    }

    acceptor_type acceptor_;
    std::function< void(tcp_connection::ptr) > f_callback_;

}; // class tcp_listener

//###########################################################################

/// \brief The base_processor class
class processor : private boost::noncopyable {

private:

    using ios_ptr = std::shared_ptr<boost::asio::io_service>;
    using work_ptr = std::shared_ptr<boost::asio::io_service::work>;
    using listener_ptr = std::shared_ptr<tcp_listener>;
    using duration_type = boost::asio::deadline_timer::duration_type;
    using time_type = boost::asio::deadline_timer::time_type;
    using listeners_type = std::map<uint32_t, listener_ptr>;

public:

    static auto& get(){
        static processor base_proc;
        return base_proc;
    }

    template <class F>
    inline void push_task(F&& f) {
        ios_->post(detail::task_wrapped<F>::make(std::forward<F>(f)));
    }

    void start(std::size_t threads_count) {
        for (std::size_t i = 0; i < threads_count; ++i)
            threads_pool_.create_thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios_)));
    }

    void stop() {
        ios_->stop();
    }

    void wait(){
        threads_pool_.join_all();
    }

    template <class F>
    void run_after(duration_type duration, F&& f) {
        detail::timer_task<F>::make(*ios_, duration, std::forward<F>(f)).launch();
    }

    template <class F>
    void run_at(time_type time, F&& f) {
        detail::timer_task<F>::make(*ios_, time, std::forward<F>(f)).launch();
    }

    template <class F>
    const auto & add_listener(const std::string & address, uint32_t port, F&& f) {
        listeners_type::const_iterator it = listeners_.find(port);
        if (it != listeners_.end()) {
            throw std::runtime_error("Port '"+ boost::lexical_cast<std::string>(port) + "' already created");
        }

        listeners_.insert({
                              port,
                              std::make_shared<tcp_listener>(*ios_,address, boost::lexical_cast<std::string>(port),std::forward<F>(f))
                          });

        return listeners_.at(port);
    }

    auto remove_listener(uint32_t port) {
        listeners_type::iterator it = listeners_.find(port);
        if (it == listeners_.end()) {
            throw std::runtime_error("No listener for port '"+ boost::lexical_cast<std::string>(port) + "' created");
        }

        auto listener = it->second;
        listeners_.erase(it);
        return listener;
    }

    template<class F>
    tcp_connection::ptr create_connection(const std::string & address, uint32_t port, F&& f) {
        boost::asio::ip::tcp::resolver resolver(*ios_);
        boost::asio::ip::tcp::resolver::query query(address, boost::lexical_cast<std::string>(port));

        boost::system::error_code ec;

        auto resolved = resolver.resolve(query, ec);
        if(ec){
            fail(ec, "resolve");
            return {};
        }

        boost::asio::ip::tcp::endpoint endpoint = *resolved;
        return std::make_shared<tcp_connection>(*ios_, endpoint, std::forward<F>(f));
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

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
    using native_handle_type = boost::asio::posix::stream_descriptor::native_handle_type;
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
    using native_handle_type = boost::asio::windows::stream_handle::native_handle_type;
#else
#error "Neither BOOST_ASIO_HAS_LOCAL_SOCKETS nor BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE is defined"
#endif

    void assign_in_descriptor(const native_handle_type & descriptor){
        in.assign(descriptor);
    }

    void assign_out_descriptor(const native_handle_type & descriptor){
        out.assign(descriptor);
    }

    template<class F>
    std::size_t read_from_stream(std::string & value, F && completion){
        if(in.is_open())
            return boost::asio::read(in, boost::asio::buffer(value), std::forward<F>(completion));

        return 0;
    }

    template<class F>
    std::size_t write_to_stream(const std::string & value, F && completion){
        if(out.is_open())
            return boost::asio::write(out, boost::asio::buffer(value), std::forward<F>(completion));

        return 0;
    }

private:

    ios_ptr ios_;
    work_ptr   work_;
    boost::thread_group threads_pool_;
    listeners_type listeners_;
    boost::asio::signal_set signals_;
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
    boost::asio::posix::stream_descriptor in;
    boost::asio::posix::stream_descriptor out;
#elif defined(BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
    boost::asio::windows::stream_handle in;
    boost::asio::windows::stream_handle out;
#endif
    std::function<void(int)> signal_handlers_;

    processor()
        : ios_{std::make_shared<boost::asio::io_service>()}
        , work_{std::make_shared<boost::asio::io_service::work>(*ios_)},
          signals_{*ios_},
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
          in{*ios_, ::dup(STDIN_FILENO)},
          out{*ios_, ::dup(STDOUT_FILENO)}
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
