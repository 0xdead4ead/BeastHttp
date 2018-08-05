#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <session.hpp>
#include <list_cb.hpp>

namespace http {

/// \brief Routing module class
template<class ReqBody>
class router{

    using self = router<ReqBody>;
    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

public:

    explicit router(std::shared_ptr<resource_map_t> & resource_map_cb_p, std::shared_ptr<method_map_t> & method_map_cb_p) :
        resource_map_cb_p_{resource_map_cb_p},
        method_map_cb_p_{method_map_cb_p}
    {}

protected:

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

    void add_resource_cb(const resource_regex_t & path_to_resource, const method_t & method, list_cb_t && l){


        if(!method_map_cb_p_)
            method_map_cb_p_ = std::make_shared<method_map_t>();

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
            list_cb_p = std::make_shared<list_cb_t>();

        *list_cb_p = std::move(l);

    }

    void add_resource_cb_without_method(const resource_regex_t & path_to_resource, list_cb_t && l){

        if(!resource_map_cb_p_)
            resource_map_cb_p_ = std::make_shared<resource_map_t>();

        resource_map_cb_p_->insert({
                                       path_to_resource,
                                       typename list_cb_t::ptr()
                                   });

        auto & list_cb_p = resource_map_cb_p_->at(path_to_resource);
        if(!list_cb_p)
            list_cb_p = std::make_shared<list_cb_t>();

        *list_cb_p = std::move(l);

    }

    void use(const resource_regex_t & path_to_resource, const self & other){
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
    }

    std::shared_ptr<resource_map_t> & resource_map_cb_p_;
    std::shared_ptr<method_map_t> & method_map_cb_p_;

}; // router class


template<class ReqBody>
class basic_router : public router<ReqBody>{

    using base_t = router<ReqBody>;
    using self = basic_router<ReqBody>;
    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

public:

    using ref = std::add_lvalue_reference_t<self>;
    using cref = std::add_lvalue_reference_t<std::add_const_t<self>>;

    basic_router()
        : base_t{resource_map_cb_p_, method_map_cb_p_}
    {}

    explicit basic_router(std::shared_ptr<resource_map_t> & resource_map_cb_p, std::shared_ptr<method_map_t> & method_map_cb_p)
        : base_t(resource_map_cb_p, method_map_cb_p){}

    /// Callback signature : template<class Message, class Session /*, class Next (optional)*/>
    ///                     void (const Message & message, Session & session /*, Next & next (optional)*/)
    /// \brief Adds a handler for GET method
    template<class... Callback>
    void get(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::get, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for POST method
    template<class... Callback>
    void post(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::post, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for PUT method
    template<class... Callback>
    void put(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::put, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for HEAD method
    template<class... Callback>
    void head(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::head, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for DELETE method
    template<class... Callback>
    void delete_(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::delete_, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for OPTIONS method
    template<class... Callback>
    void options(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::options, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for CONNECT method
    template<class... Callback>
    void connect(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::connect, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for TRACE method
    template<class... Callback>
    void trace(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::trace, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for COPY method
    template<class... Callback>
    void copy(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::copy, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for LOCK method
    template<class... Callback>
    void lock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::lock, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for MKCOL method
    template<class... Callback>
    void mkcol(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::mkcol, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for MOVE method
    template<class... Callback>
    void move(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::move, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for PROPFIND method
    template<class... Callback>
    void propfind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::propfind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for PROPPATCH method
    template<class... Callback>
    void proppatch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::proppatch, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for SEARCH method
    template<class... Callback>
    void search(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::search, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for UNLOCK method
    template<class... Callback>
    void unlock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::unlock, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for BIND method
    template<class... Callback>
    void bind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::bind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for REBIND method
    template<class... Callback>
    void rebind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::rebind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for UNBIND method
    template<class... Callback>
    void unbind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::unbind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for ACL method
    template<class... Callback>
    void acl(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb(path_to_resource, method_t::acl, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }
    /// \brief Adds a handler for the requested resource by default
    /// \note If the handler for the requested resource with method is not found, this on is called
    template<class... Callback>
    void all(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers){
        base_t::add_resource_cb_without_method(path_to_resource, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
    }

    void use(const resource_regex_t & path_to_resource, cref other){
        base_t::use(path_to_resource, other);
    }

private:

    std::shared_ptr<resource_map_t> resource_map_cb_p_;
    std::shared_ptr<method_map_t> method_map_cb_p_;

}; //basic_router class



/// \brief Routing module class for chaining process
template<class ReqBody>
class chain_router : public router<ReqBody>{

    using base_t = router<ReqBody>;
    using self = chain_router<ReqBody>;
    using list_cb_t = list_cb<boost::beast::http::request<ReqBody>, session<true, ReqBody>>;
    using resource_map_t = boost::unordered_map<resource_regex_t,typename list_cb_t::ptr>;
    using method_map_t = std::map<method_t, resource_map_t>;

public:

    using ref = std::add_lvalue_reference_t<self>;
    using cref = std::add_lvalue_reference_t<std::add_const_t<self>>;

    chain_router()
        : base_t{resource_map_cb_p_, method_map_cb_p_}
    {}

    explicit chain_router(std::shared_ptr<resource_map_t> & resource_map_cb_p, std::shared_ptr<method_map_t> & method_map_cb_p)
        : base_t{resource_map_cb_p, method_map_cb_p}
    {}

    ref route(const resource_regex_t & path_to_resource){
        save_to_res_regex(path_to_resource);
        return *this;
    }

    template<class... Callback>
    ref get(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::get, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref post(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::post, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref put(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::put, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref head(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::head, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref delete_(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::delete_, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref options(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::options, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref connect(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::connect, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref trace(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::trace, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref copy(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::copy, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref lock(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::lock, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref mkcol(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::mkcol, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref move(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::move, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref propfind(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::propfind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref proppatch(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::proppatch, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref search(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::search, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref unlock(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::unlock, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref bind(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::bind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref rebind(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::rebind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref unbind(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::unbind, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref acl(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb(tmp_res_regex_, method_t::acl, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    template<class... Callback>
    ref all(Callback && ... on_resource_handlers) & {
        base_t::add_resource_cb_without_method(tmp_res_regex_, list_cb_t{base_t::prepare_list_cb(on_resource_handlers...)});
        return *this;
    }

    /// \brief Save to to temporary regex resource
    inline void save_to_res_regex(const resource_regex_t & path_to_resource){
        tmp_res_regex_ = path_to_resource;
    }

    void use(const resource_regex_t & path_to_resource, cref other){
        base_t::use(path_to_resource, other);
    }

private:

    // Temporary variable for storing a regular expression for implementing a chain task of routes
    resource_regex_t tmp_res_regex_;
    std::shared_ptr<resource_map_t> resource_map_cb_p_;
    std::shared_ptr<method_map_t> method_map_cb_p_;

}; // chain_router class

}

#endif // ROUTER_HPP
