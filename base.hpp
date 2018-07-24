#ifndef BASE_HPP
#define BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/system/error_code.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <iostream>

namespace http {

namespace base {

void out(const std::string & info){
    std::cout << info << std::endl;
}

void fail(const std::string & info){
    std::cerr << info << std::endl;
}

void fail(const boost::system::error_code & ec, const std::string & info){
    std::cerr << info << " : " << ec.message() << std::endl;
}

/// \brief class storing a task
/// \tparam functor
template <class F>
struct task_wrapped {

private:

    F f_;

public:

    explicit task_wrapped(const F& f)
        : f_(f)
    {}

    void operator()() const {
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
    using timer_ptr = boost::shared_ptr<boost::asio::deadline_timer>;
    timer_ptr timer_;

public:

    template <class Time>
    explicit timer_task(boost::asio::io_service& ios, const Time& duration_or_time, const F& f)
        : base_type(f),
          timer_(boost::make_shared<boost::asio::deadline_timer>(
                     boost::ref(ios), duration_or_time))
    {}

    void launch() const {
        timer_->async_wait(*this);
    }

    void operator()(const boost::system::error_code& error) const {
        if (!error)
            base_type::operator()();
        else
            fail(error, "http::base::timer_task");
    }

    template <class Time>
    static inline auto make(boost::asio::io_service& ios, const Time& duration_or_time, const F& f)
    {
        return timer_task<F>(ios, duration_or_time, f);
    }

}; // class timer_task

//###########################################################################

/// \brief The tcp_connection class
class tcp_connection : private boost::noncopyable {

public:

    using ptr = boost::shared_ptr<tcp_connection>;

    explicit tcp_connection(boost::asio::io_service& ios)
        : socket_(ios), strand_(socket_.get_executor())
    {}

    template<class F>
    explicit tcp_connection(
            boost::asio::io_service& ios,
            const boost::asio::ip::tcp::endpoint& endpoint, F&& f)
        : socket_(ios), strand_(socket_.get_executor())
    {
        socket_.async_connect(endpoint, boost::forward<F>(f));
    }

    template <class F, class R>
    void async_write(R& msg, F&& f)
    {
        boost::beast::http::async_write(socket_, msg,
            boost::asio::bind_executor(
                strand_, boost::forward<F>(f)));
    }

    template <class F, class B, class R>
    void async_read(B& buf, R& msg, F&& f)
    {
        boost::beast::http::async_read(socket_, buf, msg,
            boost::asio::bind_executor(
                strand_, boost::forward<F>(f)));
    }

    void shutdown() {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket_.close();
    }

    boost::asio::ip::tcp::socket & socket(){
        return socket_;
    }

private:

    boost::asio::ip::tcp::socket socket_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;

}; // tcp_connection class

//###########################################################################

/// \brief The tcp_listener class
class tcp_listener: public boost::enable_shared_from_this<tcp_listener> ,
        private boost::noncopyable
{

    using acceptor_type = boost::asio::ip::tcp::acceptor;

public:

    template <class F>
    tcp_listener(boost::asio::io_service& io_service, const std::string & address, const std::string & port, F&& f)
        : acceptor_(io_service)
        , f_(boost::forward<F>(f))
    {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(address, port);
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
    }

    void stop() {
        acceptor_.close();
    }

    void accept_start() {
        if (!acceptor_.is_open()) {
            return;
        }

        auto new_connection = boost::make_shared<tcp_connection>(acceptor_.get_io_service());
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
        accept_start();
        if (!error)
            f_(new_connection); // running task
        else
            std::cerr << "http::base::tcp_listener::handle_accept " << error.message() << std::endl;
    }

    acceptor_type acceptor_;
    boost::function< void(tcp_connection::ptr) > f_;

}; // class tcp_listener

//###########################################################################

/// \brief The base_processor class
class processor : private boost::noncopyable {

private:

    using ios_ptr = boost::shared_ptr<boost::asio::io_service>;
    using work_ptr = boost::shared_ptr<boost::asio::io_service::work>;
    using listener_ptr = boost::shared_ptr<tcp_listener>;
    using duration_type = boost::asio::deadline_timer::duration_type;
    using time_type = boost::asio::deadline_timer::time_type;
    using listeners_type = std::map<uint32_t, listener_ptr>;

public:

    static processor& get(){
        static processor base_proc;
        return base_proc;
    }

    template <class F>
    inline void push_task(F&& f) {
        ios_->post(task_wrapped<F>::make(boost::forward<F>(f)));
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
        timer_task<F>::make(*ios_, duration, boost::forward<F>(f)).launch();
    }

    template <class F>
    void run_at(time_type time, F&& f) {
        timer_task<F>::make(*ios_, time, boost::forward<F>(f)).launch();
    }

    template <class F>
    const listener_ptr & add_listener(const std::string & address, uint32_t port, F&& f) {
        listeners_type::const_iterator it = listeners_.find(port);
        if (it != listeners_.end()) {
            throw std::logic_error(
                "Such listener for port '"
                + boost::lexical_cast<std::string>(port)
                + "' already created"
            );
        }

        listeners_.insert(std::make_pair(port,
                                         boost::make_shared<tcp_listener>(*ios_,
                                                                          address,
                                                                          boost::lexical_cast<std::string>(port),
                                                                          boost::forward<F>(f))));

        return listeners_.at(port);
    }

    listener_ptr remove_listener(uint32_t port) {
        listeners_type::iterator it = listeners_.find(port);
        if (it == listeners_.end()) {
            throw std::logic_error(
                "No listener for port '"
                + boost::lexical_cast<std::string>(port)
                + "' created"
            );
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
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query, ec);

        if(ec){
            fail(ec, "resolve");
            return {};
        }

        return boost::make_shared<tcp_connection>(*ios_, endpoint, boost::forward<F>(f));
    }

private:

    ios_ptr ios_;
    work_ptr   work_;
    boost::thread_group threads_pool_;
    listeners_type listeners_;

    processor()
        : ios_(boost::make_shared<boost::asio::io_service>())
        , work_(boost::make_shared<boost::asio::io_service::work>(*ios_))
    {}

}; // class processor

} // namespace base

} // namespace http

#endif // BASE_HPP
