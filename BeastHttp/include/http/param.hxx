#if not defined BEASTHTTP_PARAM_HXX
#define BEASTHTTP_PARAM_HXX

#include <http/base/regex.hxx>
#include <http/base/traits.hxx>

#include <tuple>

#include <boost/lexical_cast.hpp>

#define BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC \
    if (shared_block_p_->rp_iter_ \
            != shared_block_p_->regex_pack.cbegin()) { \
        shared_block_p_->rp_iter_ = shared_block_p_->regex_pack.cend(); \
        shared_block_p_->rp_iter_--; \
        shared_block_p_->cur_pos_cb_ = shared_block_p_->regex_pack.size() - 1; \
    } \
    std::smatch what; \
    const auto& target = request.target().to_string(); \
    if (shared_block_p_->regex_.match( \
                *(shared_block_p_->rp_iter_), target, what)) \
        for (size_t i = 1; i < what.size(); i++) \
            shared_block_p_->str_args.push_back(what[int(i)]);

#define BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED \
    for (std::size_t i = shared_block_p_->cur_pos_cb_; i < it.pos(); i++) { \
        shared_block_p_->rp_iter_++; \
        if (shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cend()) \
            shared_block_p_->rp_iter_--; \
        shared_block_p_->cur_pos_cb_++; \
    } \
    if (shared_block_p_->rp_iter_ == shared_block_p_->regex_pack.cbegin()) \
        shared_block_p_->rp_iter_++; \
    std::smatch what; \
    const auto& target = request.target().to_string(); \
    if (shared_block_p_->regex_.match( \
                *(shared_block_p_->rp_iter_), target, what)) \
        for (size_t i = 1; i < what.size(); i++) \
            shared_block_p_->str_args.push_back(what[int(i)]);

#define BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS \
    shared_block_ptr shared_block_p_; \
    F f_; \
    explicit \
    paramcb(const shared_block_ptr& shared_block_p, const F& f) \
        : shared_block_p_{shared_block_p}, \
          f_{f} \
    {} \
    explicit \
    paramcb(const shared_block_ptr& shared_block_p, F&& f) \
        : shared_block_p_{shared_block_p}, \
          f_{std::move(f)} \
    {}

namespace _0xdead4ead {
namespace http {

template<class>
class basic_router;

template<class>
class chain_router;

namespace param {

template<class, class>
class base;

template<class, class>
class impl;

template<class... Types>
class pack
{
    using self_type = pack;

    using regex_type = http::base::regex;

    static constexpr unsigned max_count = 12;

    template<class T, unsigned>
    struct Check
    {
        static_assert (std::is_default_constructible<T>::value,
                       "Default structor is missing!");
    };

    using TypeList = http::base::traits::TypeList<Types...>;

    template<unsigned Index>
    using get_ = http::base::traits::Get<TypeList, Index>;

    http::base::traits::ForEach<TypeList, Check> dummy;

    template<class, class>
    friend class base;

public:

    using Tuple = std::tuple<Types...>;

    template<class Router>
    static impl<Router, self_type>
    provide(Router& router, typename regex_type::flag_type flags)
    {
        return impl<Router, self_type>{router, flags};
    }

}; // class pack

template<class Router, class Pack>
class base
{
    using self_type = base;

protected:

    using router_type = Router;

    using pack_type = Pack;

    using resource_regex_type = typename router_type::resource_regex_type;

    using resource_regex_pack_type = std::vector<resource_regex_type>;

    using regex_type = typename pack_type::regex_type;

    using tuple_type = typename pack_type::Tuple;

    using request_type = typename router_type::request_type;

    using storage_type = typename router_type::storage_type;

    using iterator_type = typename storage_type::iterator_type;

    using session_type = typename router_type::session_type;

    using context_type = typename session_type::context_type;

    struct shared_block : public std::enable_shared_from_this<shared_block>
    {
        router_type& router_;
        size_t cur_pos_cb_ = 0;
        resource_regex_pack_type regex_pack;
        typename resource_regex_pack_type::const_iterator rp_iter_;
        std::vector<std::string> str_args;
        regex_type regex_;

        shared_block(router_type& router, typename regex_type::flag_type flags)
            : router_{router}, regex_{flags}
        {}

        void reset(){
            rp_iter_ = regex_pack.cbegin();
            cur_pos_cb_ = 0;
            str_args.clear();
        }

    }; // struct shared_block

    using shared_block_type = shared_block;

    using shared_block_ptr = std::shared_ptr<shared_block_type>;

    shared_block_ptr shared_block_p_;

    base(const shared_block_ptr& shared_block_p)
        : shared_block_p_{shared_block_p}
    {}

    /// \brief Split regex path to '/' character
    void
    split_regex(resource_regex_type path_to_resource)
    {
        if (path_to_resource.empty())
            return;

        shared_block_p_->regex_pack.clear();
        shared_block_p_->regex_pack.push_back(path_to_resource);
        constexpr char delim = '/';
        size_t pos = 0;
        std::string token;
        while(true) {
            pos = path_to_resource.find(delim);
            if (pos == 0)
                pos = path_to_resource.find(delim, pos + 1);
            if (pos != std::string::npos) {
                token = path_to_resource.substr(0, pos);
                shared_block_p_->regex_pack.push_back(token);
                path_to_resource.erase(0, pos);
            }
            else {
                token = path_to_resource;
                shared_block_p_->regex_pack.push_back(token);
                break;
            }
        }
        shared_block_p_->rp_iter_ = shared_block_p_->regex_pack.cbegin();
    }

    template<class F, unsigned Count>
    struct paramcb
    {
        static_assert (Count != 0, "Incorrect value, pack is empty!");

        static_assert (Count < pack_type::max_count + 1,
                       "Pack types limit!");

    }; // struct paramcb

    template<class F>
    struct paramcb<F, 1>
    {
        static constexpr unsigned count = 1;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 1>

    template<class F>
    struct paramcb<F, 2>
    {
        static constexpr unsigned count = 2;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    };  // struct paramcb<F, 2>

    template<class F>
    struct paramcb<F, 3>
    {
        static constexpr unsigned count = 3;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 3>

    template<class F>
    struct paramcb<F, 4>
    {
        static constexpr unsigned count = 4;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 4>

    template<class F>
    struct paramcb<F, 5>
    {
        static constexpr unsigned count = 5;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 5>

    template<class F>
    struct paramcb<F, 6>
    {
        static constexpr unsigned count = 6;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 6>

    template<class F>
    struct paramcb<F, 7>
    {
        static constexpr unsigned count = 7;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 7>

    template<class F>
    struct paramcb<F, 8>
    {
        static constexpr unsigned count = 8;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 8>

    template<class F>
    struct paramcb<F, 9>
    {
        static constexpr unsigned count = 9;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 9>

    template<class F>
    struct paramcb<F, 10>
    {
        static constexpr unsigned count = 10;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 9) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 9) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 10>

    template<class F>
    struct paramcb<F, 11>
    {
        static constexpr unsigned count = 11;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                    boost::lexical_cast<typename pack_type::template get_<10>::type>(shared_block_p_->str_args[10])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 9) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 10) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                    boost::lexical_cast<typename pack_type::template get_<10>::type>(shared_block_p_->str_args[10])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 9) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 10) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 11>

    template<class F>
    struct paramcb<F, 12>
    {
        static constexpr unsigned count = 11;

        BEASTHTTP_DEFINE_MEMBERS_AND_STRUCTS

        void
        operator()(request_type request, context_type context)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_CLASSIC

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                    boost::lexical_cast<typename pack_type::template get_<10>::type>(shared_block_p_->str_args[10]),
                    boost::lexical_cast<typename pack_type::template get_<11>::type>(shared_block_p_->str_args[11])
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                    boost::lexical_cast<typename pack_type::template get_<10>::type>(shared_block_p_->str_args[10]),
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 9) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 10) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 11) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), tuple_type{});
            }
            shared_block_p_->reset();
        }

        void
        operator()(request_type request, context_type context, iterator_type it)
        {
            BEASTHTTP_IMPL_PARAM_PROCESS_ADVANCED

            if (shared_block_p_->str_args.size() >= count) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                    boost::lexical_cast<typename pack_type::template get_<10>::type>(shared_block_p_->str_args[10]),
                    boost::lexical_cast<typename pack_type::template get_<11>::type>(shared_block_p_->str_args[11])
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 1) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                    boost::lexical_cast<typename pack_type::template get_<10>::type>(shared_block_p_->str_args[10]),
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 2) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                    boost::lexical_cast<typename pack_type::template get_<9>::type>(shared_block_p_->str_args[9]),
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 3) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                    boost::lexical_cast<typename pack_type::template get_<8>::type>(shared_block_p_->str_args[8]),
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 4) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                    boost::lexical_cast<typename pack_type::template get_<7>::type>(shared_block_p_->str_args[7]),
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 5) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                    boost::lexical_cast<typename pack_type::template get_<6>::type>(shared_block_p_->str_args[6]),
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 6) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                    boost::lexical_cast<typename pack_type::template get_<5>::type>(shared_block_p_->str_args[5]),
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 7) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                    boost::lexical_cast<typename pack_type::template get_<4>::type>(shared_block_p_->str_args[4]),
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 8) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                    boost::lexical_cast<typename pack_type::template get_<3>::type>(shared_block_p_->str_args[3]),
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 9) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                    boost::lexical_cast<typename pack_type::template get_<2>::type>(shared_block_p_->str_args[2]),
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 10) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                    boost::lexical_cast<typename pack_type::template get_<1>::type>(shared_block_p_->str_args[1]),
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else if (shared_block_p_->str_args.size() == count - 11) {
                tuple_type args{
                    boost::lexical_cast<typename pack_type::template get_<0>::type>(shared_block_p_->str_args[0]),
                            typename pack_type::template get_<1>::type{},
                            typename pack_type::template get_<2>::type{},
                            typename pack_type::template get_<3>::type{},
                            typename pack_type::template get_<4>::type{},
                            typename pack_type::template get_<5>::type{},
                            typename pack_type::template get_<6>::type{},
                            typename pack_type::template get_<7>::type{},
                            typename pack_type::template get_<8>::type{},
                            typename pack_type::template get_<9>::type{},
                            typename pack_type::template get_<10>::type{},
                            typename pack_type::template get_<11>::type{}
                };
                f_(std::move(request), std::move(context), std::move(it), std::move(args));
            }
            else {
                f_(std::move(request), std::move(context), std::move(it), tuple_type{});
            }
        }

    }; // struct paramcb<F, 12>

    template<class F, unsigned Count>
    using paramcb_type = paramcb<F, Count>;

}; // class base

template<
        class Session,
        class... Types>
class impl<basic_router<Session>, pack<Types...>>
        : private base<basic_router<Session>, pack<Types...>>
{
    using self_type = impl;

    using base_type = base<basic_router<Session>, pack<Types...>>;

public:

    using resource_regex_type = typename base_type::resource_regex_type;

    using router_type = basic_router<Session>;

    using regex_type = typename base_type::regex_type;

    using request_type = typename base_type::request_type;

    using iterator_type = typename base_type::iterator_type;

    using context_type = typename base_type::context_type;

    using tuple_type = typename base_type::tuple_type;

    explicit
    impl(router_type& router, typename regex_type::flag_type flags,
         typename std::enable_if<http::base::traits::Conjunction<std::is_default_constructible<Types>...>::value, int>::type = 0)
        : base_type{std::make_shared<typename base_type::shared_block_type>(router, flags)}
    {
    }

    template<class... OnRequest>
    auto
    get(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.get(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    post(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.post(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    put(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.put(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    head(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.head(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    delete_(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.delete_(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    options(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.options(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    connect(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.connect(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    trace(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.trace(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    copy(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.copy(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    lock(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.lock(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    mkcol(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.mkcol(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    move(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.move(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    propfind(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.propfind(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    proppatch(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.proppatch(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    search(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.search(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    unlock(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.unlock(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    bind(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.bind(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    rebind(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.rebind(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    unbind(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.unbind(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    acl(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.acl(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    report(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.report(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    mkactivity(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.mkactivity(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    checkout(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.checkout(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    merge(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.merge(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    msearch(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.msearch(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    notify(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.notify(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    subscribe(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.subscribe(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    unsubscribe(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.unsubscribe(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    patch(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.patch(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    purge(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.purge(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    mkcalendar(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.mkcalendar(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    link(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.link(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    unlink(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.unlink(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

    template<class... OnRequest>
    auto
    all(const resource_regex_type& path_to_resource, OnRequest&&... on_request) &&
    -> decltype (void(typename std::enable_if<http::base::traits::TryInvokeConjunction<
                      sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                 void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}))
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.all(
                    path_to_resource, typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                              {base_type::shared_block_p_, std::forward<OnRequest>(on_request)}...);
    }

}; // class impl<basic_router<Session>, Pack<Types...>>

template<
        class Session,
        class... Types>
class impl<chain_router<Session>, pack<Types...>>
        : private base<chain_router<Session>, pack<Types...>>
{
    using self_type = impl;

    using base_type = base<chain_router<Session>, pack<Types...>>;

public:

    using resource_regex_type = typename base_type::resource_regex_type;

    using router_type = chain_router<Session>;

    using regex_type = typename base_type::regex_type;

    using chain_node_type = typename router_type::chain_node_type;

    using request_type = typename base_type::request_type;

    using iterator_type = typename base_type::iterator_type;

    using context_type = typename base_type::context_type;

    using tuple_type = typename base_type::tuple_type;

    struct node : private chain_node_type
    {
        using self_type = node;

        node(const typename base_type::shared_block_ptr& shared_block_p)
            : chain_node_type{shared_block_p->router_},
              shared_block_p_{shared_block_p}
        {}

        template<class... OnRequest>
        auto
        get(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::get(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        post(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::post(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        put(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::put(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        head(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::head(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        delete_(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::delete_(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        options(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::options(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        connect(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::connect(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        trace(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::trace(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        copy(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::copy(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        lock(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::lock(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        mkcol(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::mkcol(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        move(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::move(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        propfind(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::propfind(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        proppatch(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::proppatch(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        search(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::search(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        unlock(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::unlock(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        bind(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::bind(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        rebind(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::rebind(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        unbind(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::unbind(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        acl(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::acl(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        report(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::report(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        mkactivity(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::mkactivity(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        checkout(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::checkout(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        merge(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::merge(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        msearch(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::msearch(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        notify(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::notify(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        subscribe(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::subscribe(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        unsubscribe(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::unsubscribe(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        patch(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::patch(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        purge(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::purge(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        mkcalendar(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::mkcalendar(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        link(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::link(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        unlink(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::unlink(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

        template<class... OnRequest>
        auto
        all(OnRequest&&... on_request)
        -> decltype (typename std::enable_if<http::base::traits::TryInvokeConjunction<
                          sizeof... (OnRequest) - 1, void (request_type, context_type, iterator_type, tuple_type),
                     void (request_type, context_type, tuple_type), OnRequest...>::value, int>::type{}, std::declval<node&>())
        {
            chain_node_type::all(typename base_type::template paramcb_type<OnRequest, sizeof... (Types)>
                                    {shared_block_p_, std::forward<OnRequest>(on_request)}...);
            return *this;
        }

    private:

        typename base_type::shared_block_ptr shared_block_p_;

    }; // struct node

    explicit
    impl(router_type& router, typename regex_type::flag_type flags)
        : base_type{std::make_shared<typename base_type::shared_block_type>(router, flags)}
    {}

    node
    route(const resource_regex_type& path_to_resource) &&
    {
        base_type::split_regex(path_to_resource);
        base_type::shared_block_p_->router_.route(path_to_resource);
        return node{base_type::shared_block_p_};
    }

}; // class impl<chain_router<Session>, Pack<Types...>>

} // namespace param
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_PARAM_HXX
