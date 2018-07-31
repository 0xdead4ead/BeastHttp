#ifndef SERVER_HPP
#define SERVER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <session.hpp>
#include <list_cb.hpp>


namespace http {


/// \brief HTTP server class
/// \tparam Body type for request message
template<class ReqBody>
class server_impl{

public:

    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using method_map_t = std::map<method_t, boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>>;

    explicit server_impl() : status_{false},
        def_all_cb_p_{nullptr},
        method_map_cb_p_{nullptr}
    {}

    /// \brief Adds a handler for the requested resource
    /// \param Regular expression. Request content
    /// \param Method (GET, POST and etc.)
    /// \param Handler on receipt
    /// Callback signature : template<class Message, class Session /*, class NextCallback (optional)*/>
    ///                     void (const Message & message, const Session & session /*, NextCallback & next (optional)*/)
    template<class... Callback>
    void route(const resource_regex_t & path_to_resource, const method_t & method, Callback && ... on_resource_handlers){
        if(status_)
            return base::fail("Adding a handler is not available when the server is running");

        if(!method_map_cb_p_)
            method_map_cb_p_ = boost::make_shared<method_map_t>();

        method_map_cb_p_->insert({
                                     method,
                                     boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>()
                                 });

        auto & resource_map_cb = method_map_cb_p_->at(method);

        resource_map_cb.insert({
                                   path_to_resource,
                                   typename list_cb_t::ptr()
                               });

        auto & list_cb_p = resource_map_cb.at(path_to_resource);
        if(!list_cb_p)
            list_cb_p = boost::make_shared<list_cb_t>();

        *list_cb_p = prepare_list_cb(on_resource_handlers...);

    }

    /// \brief Adds default handler
    /// \note If the handler for the requested resource is not found, this on is called
    /// \param Handler on receipt
    /// Callback signature : template<class Message, class Session /*, class NextCallback (optional)*/>
    ///                     void (const Message & message, const Session & session /*, NextCallback & next (optional)*/)
    template<class... Callback>
    void all_route(Callback && ... default_handlers){
        if(status_)
            return base::fail("Adding a handler is not available when the server is running");

        if(!def_all_cb_p_)
            def_all_cb_p_ = boost::make_shared<list_cb_t>();

        *def_all_cb_p_ = prepare_list_cb(default_handlers...);
    }

    /// \brief Start accepting incoming connections
    /// \param Listening interface
    /// \param port
    /// \param Handler on new incoming connection
    /// Callback signature : template<class Session>
    ///                     void (const Session & session)
    template<class Callback>
    void listen(const std::string & address, uint32_t port, Callback && on_accept_handler){
        if(status_)
            return base::fail("server is already running");

        base::processor::get().add_listener(address, port,
                                            boost::bind(&session<true, ReqBody>::template on_accept<Callback>,
                                                        boost::placeholders::_1,
                                                        def_all_cb_p_,
                                                        method_map_cb_p_,
                                                        boost::forward<Callback>(on_accept_handler)
                                                        )
                                            )->accept_start();
        status_ = true;
    }

private:

    template <typename T, typename F1, typename F2, size_t Index = 0>
    void tuple_cb_for_each(const T& tpl, const F1& f1, const F2& f2) {

        constexpr auto tuple_size = std::tuple_size_v<T>;
        BOOST_STATIC_ASSERT(tuple_size != 0);

        const auto& value = std::get<Index>(tpl);
        if constexpr(Index + 1 == tuple_size){
            f1(value);
        }else{
            f2(value);
            tuple_cb_for_each<T, F1, F2, Index + 1>(tpl, f1, f2);
        }
    }

    template<class... Callback>
    auto prepare_list_cb(const Callback & ... handlers){

        typename list_cb_t::L cb_l;

        auto tuple_cb = std::tie(handlers...);

        tuple_cb_for_each(tuple_cb, [&cb_l](const auto& value){
            cb_l.push_back(
                        typename list_cb_t::F(
                            boost::bind<void>(
                                value,
                                boost::placeholders::_1,
                                boost::placeholders::_2
                                )
                            )
                        );
        }, [&cb_l](const auto& value){
            cb_l.push_back(
                        typename list_cb_t::F(
                            boost::bind<void>(
                                value,
                                boost::placeholders::_1,
                                boost::placeholders::_2,
                                boost::placeholders::_3
                                )
                            )
                        );
        });

        return cb_l;

    }

    bool status_;
    typename list_cb_t::ptr def_all_cb_p_;
    boost::shared_ptr<method_map_t> method_map_cb_p_;

}; // server_impl class

using server = server_impl<boost::beast::http::string_body>;

} // namespace http

#endif // SERVER_HPP
