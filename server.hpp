#ifndef SERVER_HPP
#define SERVER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <base.hpp>
#include <session.hpp>
#include <list_cb.hpp>


namespace http {

/// \brief Routing module class
template<class ReqBody>
class router{

    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

public:

    using ref = std::add_lvalue_reference_t<router>;
    using cref = std::add_lvalue_reference_t<std::add_const_t<router>>;

    explicit router() :
        status_{false},
        resource_map_cb_p_{nullptr},
        method_map_cb_p_{nullptr}
    {}

//    router(router_cref other){
//        use(other);
//    }

//    auto& operator=(router_cref other){
//        use(other);
//        return *this;
//    }

//    ~router() = default;

    /// Callback signature : template<class Message, class Session /*, class Next (optional)*/>
    ///                     void (const Message & message, Session & session /*, Next & next (optional)*/)
    /// \brief Adds a handler for GET method
    template<class... Callback>
    void get(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb(path_to_resource, method_t::get, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for POST method
    template<class... Callback>
    void post(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb(path_to_resource, method_t::post, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for PUT method
    template<class... Callback>
    void put(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb(path_to_resource, method_t::put, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for HEAD method
    template<class... Callback>
    void head(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb(path_to_resource, method_t::head, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for DELETE method
    template<class... Callback>
    void delete_(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb(path_to_resource, method_t::delete_, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for OPTIONS method
    template<class... Callback>
    void options(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb(path_to_resource, method_t::options, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for the requested resource by default
    /// \note If the handler for the requested resource with method is not found, this on is called
    template<class... Callback>
    void all(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        return add_resource_cb_without_method(path_to_resource, list_cb_t{prepare_list_cb(on_resource_handlers...)});
    }

private:

    template <typename T, typename F1, typename F2, size_t Index = 0>
    void tuple_cb_for_each(const T& tpl, const F1& f1, const F2& f2) {

        constexpr auto tuple_size = std::tuple_size<T>::value;
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

protected:

    void add_resource_cb(const resource_regex_t & path_to_resource, const method_t & method, list_cb_t && l){
        if(status_)
            return base::fail("Adding a handler is not available when the server is running");

        if(!method_map_cb_p_)
            method_map_cb_p_ = boost::make_shared<method_map_t>();

        method_map_cb_p_->insert({
                                     method,
                                     resource_map_t()
                                 });

        auto & resource_map_cb = method_map_cb_p_->at(method);

        resource_map_cb.insert({
                                   path_to_resource,
                                   typename list_cb_t::ptr()
                               });

        auto & list_cb_p = resource_map_cb.at(path_to_resource);
        if(!list_cb_p)
            list_cb_p = boost::make_shared<list_cb_t>();

        *list_cb_p = std::move(l);
    }

    void add_resource_cb_without_method(const resource_regex_t & path_to_resource, list_cb_t && l){
        if(status_)
            return base::fail("Adding a handler is not available when the server is running");

        if(!resource_map_cb_p_)
            resource_map_cb_p_ = boost::make_shared<resource_map_t>();

        resource_map_cb_p_->insert({
                                       path_to_resource,
                                       typename list_cb_t::ptr()
                                   });

        auto & list_cb_p = resource_map_cb_p_->at(path_to_resource);
        if(!list_cb_p)
            list_cb_p = boost::make_shared<list_cb_t>();

        *list_cb_p = std::move(l);
    }

    ref use(const resource_regex_t & path_to_resource, cref other){
        if(other.resource_map_cb_p_)
            for(const auto & value : *other.resource_map_cb_p_){
                auto l_cb = *value.second;
                add_resource_cb_without_method(path_to_resource + value.first, std::move(l_cb));
            }


        if(other.method_map_cb_p_)
            for(const auto & value_m : *other.method_map_cb_p_){
                auto method = value_m.first;
                const auto & resource_map = value_m.second;

                for(const auto & value_r : resource_map){
                    auto l_cb = *value_r.second;
                    add_resource_cb(path_to_resource + value_r.first, method,std::move(l_cb));
                }
            }
        return *this;
    }

    bool status_;
    boost::shared_ptr<resource_map_t> resource_map_cb_p_;
    boost::shared_ptr<method_map_t> method_map_cb_p_;

}; // router class

/// \brief HTTP server class
/// \tparam Body type for request message
template<class ReqBody>
class server_impl : public router<ReqBody>{

public:

    using router_base = router<ReqBody>;

    explicit server_impl() :
        router_base()
    {}

    /// \brief Start accepting incoming connections
    /// \param Listening interface
    /// \param port
    /// \param Handler on new incoming connection
    /// Callback signature : template<class Session>
    ///                     void (const Session & session)
    template<class Callback>
    void listen(const std::string & address, uint32_t port, Callback && on_accept_handler){
        if(this->status_)
            return base::fail("server is already running");

        base::processor::get().add_listener(address, port,
                                            boost::bind(&session<true, ReqBody>::template on_accept<Callback>,
                                                        boost::placeholders::_1,
                                                        this->resource_map_cb_p_,
                                                        this->method_map_cb_p_,
                                                        boost::forward<Callback>(on_accept_handler)
                                                        )
                                            )->accept_start();
        this->status_ = true;
    }


    typename router_base::ref Router() noexcept{
        return static_cast<typename router_base::ref>(*this);
    }

    typename router_base::cref Router() const noexcept{
        return static_cast<typename router_base::ref>(*this);
    }

    typename router_base::ref use(const resource_regex_t & path_to_resource, typename router_base::cref other){
        return router_base::use(path_to_resource, other);
    }

}; // server_impl class

using server = server_impl<boost::beast::http::string_body>;

} // namespace http

#endif // SERVER_HPP
