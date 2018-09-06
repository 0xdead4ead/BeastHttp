#ifndef BEAST_HTTP_PARAM_HPP
#define BEAST_HTTP_PARAM_HPP

#include <list>
#include <memory>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <session.hpp>

//#if __cplusplus >= 201703L
//#include <variant>
//template<class... Types>
//using variant = std::variant<Types...>;
//#else
//#include <boost/variant.hpp>
//template<class... Types>
//using variant = boost::variant<Types...>;
//#endif

constexpr int max_param_args = 9;

namespace http {

// Parameter pack
template<class... Types>
struct arguments;

template<class T1>
struct arguments<T1>{
    T1 _1;
    explicit arguments(const T1 & t1) : _1{t1} {}
};

template<class T1, class T2>
struct arguments<T1, T2>{
    T1 _1;
    T2 _2;
    explicit arguments(const T1 & t1, const T2 & t2)
        : _1{t1}, _2{t2} {}
};

template<class T1, class T2, class T3>
struct arguments<T1, T2, T3>{
    T1 _1;
    T2 _2;
    T3 _3;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3)
        : _1{t1}, _2{t2}, _3{t3} {}
};

template<class T1, class T2, class T3, class T4>
struct arguments<T1, T2, T3, T4>{
    T1 _1;
    T2 _2;
    T3 _3;
    T4 _4;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4)
        : _1{t1}, _2{t2}, _3{t3}, _4{t4} {}
};

template<class T1, class T2, class T3, class T4, class T5>
struct arguments<T1, T2, T3, T4, T5>{
    T1 _1;
    T2 _2;
    T3 _3;
    T4 _4;
    T5 _5;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5)
        : _1{t1}, _2{t2}, _3{t3}, _4{t4}, _5{t5} {}
};

template<class T1, class T2, class T3, class T4, class T5, class T6>
struct arguments<T1, T2, T3, T4, T5, T6>{
    T1 _1;
    T2 _2;
    T3 _3;
    T4 _4;
    T5 _5;
    T6 _6;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6)
        : _1{t1}, _2{t2}, _3{t3}, _4{t4}, _5{t5}, _6{t6} {}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct arguments<T1, T2, T3, T4, T5, T6, T7>{
    T1 _1;
    T2 _2;
    T3 _3;
    T4 _4;
    T5 _5;
    T6 _6;
    T7 _7;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6, const T7 & t7)
        : _1{t1}, _2{t2}, _3{t3}, _4{t4}, _5{t5}, _6{t6}, _7{t7} {}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
    T1 _1;
    T2 _2;
    T3 _3;
    T4 _4;
    T5 _5;
    T6 _6;
    T7 _7;
    T8 _8;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6, const T7 & t7, const T8 & t8)
        : _1{t1}, _2{t2}, _3{t3}, _4{t4}, _5{t5}, _6{t6}, _7{t7}, _8{t8} {}
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
    T1 _1;
    T2 _2;
    T3 _3;
    T4 _4;
    T5 _5;
    T6 _6;
    T7 _7;
    T8 _8;
    T9 _9;
    explicit arguments(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4, const T5 & t5, const T6 & t6, const T7 & t7, const T8 & t8, const T9 & t9)
        : _1{t1}, _2{t2}, _3{t3}, _4{t4}, _5{t5}, _6{t6}, _7{t7}, _8{t8}, _9{t9} {}
};

template<class ReqBody>
class basic_router;

template<class ReqBody>
class chain_router;

/// \brief Split regex path to '/' character
#define IMPL_SPLIT_REGEX \
    void split_regex(resource_regex_t path_to_resource){ \
        if(path_to_resource.empty()) \
            return; \
        shared_block_p_->regex_pack.clear(); \
        shared_block_p_->regex_pack.push_back(path_to_resource); \
        constexpr char delim = '/'; \
        size_t pos = 0; \
        std::string token; \
        while(true){ \
            pos = path_to_resource.find(delim); \
            if(pos == 0) \
                pos = path_to_resource.find(delim, pos + 1); \
            if(pos != std::string::npos){ \
                token = path_to_resource.substr(0, pos); \
                shared_block_p_->regex_pack.push_back(token); \
                path_to_resource.erase(0, pos); \
            }else{ \
                token = path_to_resource; \
                shared_block_p_->regex_pack.push_back(token); \
                break; \
            } \
        } \
        shared_block_p_->rp_iter_ = shared_block_p_->regex_pack.cbegin(); \
    }

#define IMPL_PARAM_BASIC_ROUTER_LIST_VERBS \
    template<class... Callback> \
    void get(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
        base_t::split_regex(path_to_resource); \
        base_t::shared_block_p_->router_.get(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void post(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
        base_t::split_regex(path_to_resource); \
        base_t::shared_block_p_->router_.post(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void put(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
        base_t::split_regex(path_to_resource); \
        base_t::shared_block_p_->router_.put(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void head(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.head(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void delete_(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.delete_(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void options(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.options(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void connect(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.connect(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void trace(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.trace(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void copy(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.copy(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void lock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.lock(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void mkcol(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.mkcol(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void move(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.move(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void propfind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.propfind(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void proppatch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.proppatch(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void search(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.search(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void unlock(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.unlock(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void bind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.bind(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void rebind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.rebind(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void unbind(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.unbind(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void acl(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.acl(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void report(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.report(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void mkactivity(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.mkactivity(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void checkout(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.checkout(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void merge(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.merge(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void msearch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.msearch(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void notify(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.notify(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void subscribe(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.subscribe(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void unsubscribe(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.unsubscribe(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void patch(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.patch(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void purge(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.purge(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void mkcalendar(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.mkcalendar(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void link(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.link(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void unlink(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.unlink(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    } \
    template<class... Callback> \
    void all(const resource_regex_t & path_to_resource, Callback && ... on_resource_handlers) && { \
    base_t::split_regex(path_to_resource); \
    base_t::shared_block_p_->router_.all(path_to_resource, param_cb<Callback>{base_t::shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    }

#define IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS \
    template<class... Callback> \
    node_ref get(Callback && ... on_resource_handlers) { \
        base_node::get(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref post(Callback && ... on_resource_handlers) { \
        base_node::post(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref put(Callback && ... on_resource_handlers) { \
        base_node::put(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref head(Callback && ... on_resource_handlers) { \
        base_node::head(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref delete_(Callback && ... on_resource_handlers) { \
        base_node::delete_(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref options(Callback && ... on_resource_handlers) { \
        base_node::options(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref connect(Callback && ... on_resource_handlers) { \
        base_node::connect(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref trace(Callback && ... on_resource_handlers) { \
        base_node::trace(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref copy(Callback && ... on_resource_handlers) { \
        base_node::copy(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref lock(Callback && ... on_resource_handlers) { \
        base_node::lock(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref mkcol(Callback && ... on_resource_handlers) { \
        base_node::mkcol(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref move(Callback && ... on_resource_handlers) { \
        base_node::move(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref propfind(Callback && ... on_resource_handlers) { \
        base_node::propfind(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref proppatch(Callback && ... on_resource_handlers) { \
        base_node::proppatch(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref search(Callback && ... on_resource_handlers) { \
        base_node::search(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref unlock(Callback && ... on_resource_handlers) { \
        base_node::unlock(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref bind(Callback && ... on_resource_handlers) { \
        base_node::bind(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref rebind(Callback && ... on_resource_handlers) { \
        base_node::rebind(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref unbind(Callback && ... on_resource_handlers) { \
        base_node::unbind(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref acl(Callback && ... on_resource_handlers) { \
        base_node::acl(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref report(Callback && ... on_resource_handlers) { \
        base_node::report(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref mkactivity(Callback && ... on_resource_handlers) { \
        base_node::mkactivity(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref checkout(Callback && ... on_resource_handlers) { \
        base_node::checkout(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref merge(Callback && ... on_resource_handlers) { \
        base_node::merge(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref msearch(Callback && ... on_resource_handlers) { \
        base_node::msearch(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref notify(Callback && ... on_resource_handlers) { \
        base_node::notify(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref subscribe(Callback && ... on_resource_handlers) { \
        base_node::subscribe(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref unsubscribe(Callback && ... on_resource_handlers) { \
        base_node::unsubscribe(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref patch(Callback && ... on_resource_handlers) { \
        base_node::patch(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref purge(Callback && ... on_resource_handlers) { \
        base_node::purge(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref mkcalendar(Callback && ... on_resource_handlers) { \
        base_node::mkcalendar(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref link(Callback && ... on_resource_handlers) { \
        base_node::link(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref unlink(Callback && ... on_resource_handlers) { \
        base_node::unlink(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
        return *this; \
    } \
    template<class... Callback> \
    node_ref all(Callback && ... on_resource_handlers) { \
    base_node::all(param_cb<Callback>{shared_block_p_, std::forward<Callback>(on_resource_handlers)}...); \
    return *this; \
    }


template<class Router>
class param_base{

protected:

    using router_ref = std::add_lvalue_reference_t<Router>;
    using regex_pack_t = std::list<resource_regex_t>;

    struct shared_block : public std::enable_shared_from_this<shared_block>{
        router_ref router_;
        size_t cur_pos_cb_ = 0;
        regex_pack_t regex_pack;
        regex_pack_t::const_iterator rp_iter_;
        std::vector<std::string> str_args;

        shared_block(router_ref router)
            : router_{router}
        {}

        void reset(){
            rp_iter_ = regex_pack.cbegin();
            cur_pos_cb_ = 0;
            str_args.clear();
        }

    };

    using shared_block_ptr = std::shared_ptr<shared_block>;
    shared_block_ptr shared_block_p_;

    param_base(const shared_block_ptr & shared_block_p)
        : shared_block_p_{shared_block_p}
    {}

    IMPL_SPLIT_REGEX

};


// Generic parameter holder template class
template<class Session, class Router, class... Types>
class param_impl;

//################################################################## List of param class for BASIC_ROUTER

template<class Session, class T1>
class param_impl<Session, basic_router<Session>, T1>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[int(i)]);
            }

            if(shared_block_p_->str_args.size() >= 1){
                auto args = arguments<T1>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0])
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1>{T1{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 1){
                auto args = arguments<T1>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0])
                };
                f_(req, session, next, args);
            }else{
                auto args = arguments<T1>{T1{}};
                f_(req, session, next, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2>
class param_impl<Session, basic_router<Session>, T1, T2>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 2){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2>{T1{}, T2{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 2){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1])
                };
                f_(req, session, next, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{}
                };
                f_(req, session, next, args);
            }else{
                auto args = arguments<T1, T2>{T1{}, T2{}};
                f_(req, session, next, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3>
class param_impl<Session, basic_router<Session>, T1, T2, T3>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 3){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3>{T1{}, T2{}, T3{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 3){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2])
                };
                f_(req, session, next, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{}
                };
                f_(req, session, next, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{}
                };
                f_(req, session, next, args);
            }else{
                auto args = arguments<T1, T2, T3>{T1{}, T2{}, T3{}};
                f_(req, session, next, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3, class T4>
class param_impl<Session, basic_router<Session>, T1, T2, T3, T4>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 4){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4>{T1{}, T2{}, T3{}, T4{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 4){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4>{T1{}, T2{}, T3{}, T4{}};
                f_(req, session, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3, class T4, class T5>
class param_impl<Session, basic_router<Session>, T1, T2, T3, T4, T5>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 5){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5>{T1{}, T2{}, T3{}, T4{}, T5{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 5){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5>{T1{}, T2{}, T3{}, T4{}, T5{}};
                f_(req, session, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6>
class param_impl<Session, basic_router<Session>, T1, T2, T3, T4, T5, T6>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}};
                f_(req, session, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
class param_impl<Session, basic_router<Session>, T1, T2, T3, T4, T5, T6, T7>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}};
                f_(req, session, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
class param_impl<Session, basic_router<Session>, T1, T2, T3, T4, T5, T6, T7, T8>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}};
                f_(req, session, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
class param_impl<Session, basic_router<Session>, T1, T2, T3, T4, T5, T6, T7, T8, T9>
        : private param_base<basic_router<Session>>{

    using base_t = param_base<basic_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

protected:

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 9){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<T9>(shared_block_p_->str_args[8])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}, T9{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 9){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<T9>(shared_block_p_->str_args[8])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}, T9{}};
                f_(req, session, args);
            }
        }
    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t{std::make_shared<struct base_t::shared_block>(router)}
    {}

    // Implement methods list
    IMPL_PARAM_BASIC_ROUTER_LIST_VERBS

};

//################################################################## List of param class for CHAIN_ROUTER

template<class Session, class T1>
class param_impl<Session, chain_router<Session>, T1>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 1){
                auto args = arguments<T1>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0])
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1>{T1{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 1){
                auto args = arguments<T1>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0])
                };
                f_(req, session, next, args);
            }else{
                auto args = arguments<T1>{T1{}};
                f_(req, session, next, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2>
class param_impl<Session, chain_router<Session>, T1, T2>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 2){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2>{T1{}, T2{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 2){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1])
                };
                f_(req, session, next, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{}
                };
                f_(req, session, next, args);
            }else{
                auto args = arguments<T1, T2>{T1{}, T2{}};
                f_(req, session, next, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3>
class param_impl<Session, chain_router<Session>, T1, T2, T3>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 3){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3>{T1{}, T2{}, T3{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 3){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2])
                };
                f_(req, session, next, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{}
                };
                f_(req, session, next, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{}
                };
                f_(req, session, next, args);
            }else{
                auto args = arguments<T1, T2, T3>{T1{}, T2{}, T3{}};
                f_(req, session, next, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3, class T4>
class param_impl<Session, chain_router<Session>, T1, T2, T3, T4>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 4){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4>{T1{}, T2{}, T3{}, T4{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 4){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4>{T1{}, T2{}, T3{}, T4{}};
                f_(req, session, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3, class T4, class T5>
class param_impl<Session, chain_router<Session>, T1, T2, T3, T4, T5>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 5){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5>{T1{}, T2{}, T3{}, T4{}, T5{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 5){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5>{T1{}, T2{}, T3{}, T4{}, T5{}};
                f_(req, session, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6>
class param_impl<Session, chain_router<Session>, T1, T2, T3, T4, T5, T6>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}};
                f_(req, session, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
class param_impl<Session, chain_router<Session>, T1, T2, T3, T4, T5, T6, T7>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}};
                f_(req, session, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
class param_impl<Session, chain_router<Session>, T1, T2, T3, T4, T5, T6, T7, T8>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}};
                f_(req, session, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};


template<class Session, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
class param_impl<Session, chain_router<Session>, T1, T2, T3, T4, T5, T6, T7, T8, T9>
        : private param_base<chain_router<Session>>{

protected:

    using base_t = param_base<chain_router<Session>>;
    using request = boost::beast::http::request<typename Session::ReqBody>;
    using session_t = Session;
    using list_cb_t = list_cb<request, session_t>;

    template<class F>
    struct param_cb{

        typename base_t::shared_block_ptr shared_block_p_;
        F f_;

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, const F & f)
            : shared_block_p_{shared_block_p},
              f_{f}
        {}

        explicit param_cb(const typename base_t::shared_block_ptr & shared_block_p, F && f)
            : shared_block_p_{shared_block_p},
              f_{std::move(f)}
        {}

        void operator()(request & req, session_t & session){
            bool target_finish = false;

            if(shared_block_p_->rp_iter_ != shared_block_p_->regex_pack.cbegin()){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 9){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<T9>(shared_block_p_->str_args[8])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}, T9{}};
                f_(req, session, args);
            }
            shared_block_p_->reset();
        }

        void operator()(request & req, session_t & session, list_cb_t & next){
            bool target_finish = false;

            for(std::size_t i = shared_block_p_->cur_pos_cb_; i < next.pos(); i++){
                shared_block_p_->rp_iter_++;
                if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()){
                    shared_block_p_->rp_iter_--;
                    target_finish = true;
                }
                shared_block_p_->cur_pos_cb_++;
            }

            if(shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin())
                shared_block_p_->rp_iter_++;

            if(!target_finish){
                auto expr = boost::regex{*(shared_block_p_->rp_iter_)};
                boost::smatch what;
                auto target = req.target().to_string();
                if (boost::regex_match(target, what, expr))
                    for(size_t i = 1; i < what.size(); i++)
                        shared_block_p_->str_args.push_back(what[i]);
            }

            if(shared_block_p_->str_args.size() >= 9){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<T9>(shared_block_p_->str_args[8])
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 8){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<T8>(shared_block_p_->str_args[7]),
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 7){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<T7>(shared_block_p_->str_args[6]),
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 6){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<T6>(shared_block_p_->str_args[5]),
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 5){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<T5>(shared_block_p_->str_args[4]),
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 4){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<T4>(shared_block_p_->str_args[3]),
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 3){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<T3>(shared_block_p_->str_args[2]),
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 2){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<T2>(shared_block_p_->str_args[1]),
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else if(shared_block_p_->str_args.size() == 1){
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{
                    boost::lexical_cast<T1>(shared_block_p_->str_args[0]),
                    T2{},
                    T3{},
                    T4{},
                    T5{},
                    T6{},
                    T7{},
                    T8{},
                    T9{}
                };
                f_(req, session, args);
            }else{
                auto args = arguments<T1, T2, T3, T4, T5, T6, T7, T8, T9>{T1{}, T2{}, T3{}, T4{}, T5{}, T6{}, T7{}, T8{}, T9{}};
                f_(req, session, args);
            }
        }

    };

    struct param_chain_node : private chain_router<Session>::chain_node{

        using base_node = typename chain_router<Session>::chain_node;
        using node_ref = std::add_lvalue_reference_t<param_chain_node>;

        param_chain_node(const typename base_t::shared_block_ptr & shared_block_p)
            : base_node{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        // Implement methods list
        IMPL_PARAM_CHAIN_ROUTER_LIST_VERBS

    private:

        typename base_t::shared_block_ptr shared_block_p_;

    };

public:

    explicit param_impl(typename base_t::router_ref router)
        : base_t(std::make_shared<struct base_t::shared_block>(router))
    {}

    auto route(const resource_regex_t & path_to_resource) && {
        base_t::split_regex(path_to_resource);
        base_t::shared_block_p_->router_.route(path_to_resource);
        return param_chain_node{base_t::shared_block_p_};
    }

};

} // namespace http

#endif // BEAST_HTTP_PARAM_HPP
