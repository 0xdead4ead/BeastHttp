#if not defined BEASTHTTP_BASE_CB_HXX
#define BEASTHTTP_BASE_CB_HXX

#include "traits.hxx"

#include <string>
#include <functional>
#include <tuple>

#define BEASTHTTP_DECLARE_STORAGE_TEMPLATE \
    template<class Session, \
             template<typename> class Entry, \
             template<typename, typename...> class Container>

#define BEASTHTTP_DECLARE_FRIEND_CB_ITERATOR_STRUCT \
    template<class, \
             template<typename> class, \
             template<typename, typename...> class> friend struct cb::iterator;

#define BEASTHTTP_DECLARE_FRIEND_BASE_CB_EXECUTOR_CLASS \
    friend class cb::executor;

namespace _0xdead4ead {
namespace http {
namespace base {
namespace cb {

namespace details {
#if not defined (__cpp_if_constexpr)
template<std::size_t value>
using size_type = std::integral_constant<std::size_t, value>;

template <class Begin, class End, typename S, class... Elements>
struct for_each_impl
{
    void
    operator()(const std::tuple<Elements...>& tpl, const Begin& begin, const End& end)
    {
        const auto& value = std::get<S::value>(tpl);
        begin(value);
        for_each_impl<Begin, End, size_type<S() + 1>, Elements...>{}(tpl, begin, end);
    }

}; // struct for_each_impl

template <class Begin, class End, class... Elements>
struct for_each_impl<Begin, End, size_type<std::tuple_size<std::tuple<Elements...>>::value - 1>, Elements...>
{
    void
    operator()(const std::tuple<Elements...>& tpl, const Begin& begin, const End& end)
    {
        const auto& value = std::get<size_type<std::tuple_size<std::tuple<Elements...>>::value - 1>::value>(tpl);
        end(value);
        (void)begin;
    }

}; // struct for_each_impl

template<std::size_t Index, class Begin, class End, class... Elements>
void for_each(const std::tuple<Elements...>& tpl, const Begin& begin, const End& end)
{
    for_each_impl<Begin, End, size_type<Index>, Elements...>{}(tpl, begin, end);
}
#else
template<std::size_t Index, class Begin, class End, class... Elements>
void for_each(const std::tuple<Elements...>& tpl, const Begin& begin, const End& end)
{
    const auto& value = std::get<Index>(tpl);
    if constexpr (Index + 1 == std::tuple_size<std::tuple<Elements...>>::value)
        end(value);
    else {
        begin(value);
        for_each<Index + 1, Begin, End, Elements...>(tpl, begin, end);
    }
}
#endif

#if not defined __cpp_generic_lambdas
template<class Container>
struct cb_push_cxx11
{
    using container_type = Container;

    using value_type = typename container_type::value_type;

    Container& l_;

    cb_push_cxx11(Container& l) : l_{l}
    {}

    template<class F>
    void operator()(const F& value) const
    {
        l_.push_back(
                    value_type(
                        std::bind<void>(
                            value,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            std::placeholders::_3)));
    }
}; // struct cb_push_cxx11

template<class Container>
struct cb_push_fin_cxx11
{
    using container_type = Container;

    using value_type = typename container_type::value_type;

    Container& l_;

    cb_push_fin_cxx11(Container& l) : l_{l}
    {}

    template<class F>
    void operator()(const F& value) const
    {
        l_.push_back(
                    value_type(
                        std::bind<void>(
                            value,
                            std::placeholders::_1,
                            std::placeholders::_2)));
    }
}; // struct cb_push_fin_cxx11

#endif
} // namespace details

class executor
{
protected:

    template<class Request, class SessionFlesh, class Storage>
    void
    execute(Request& request, SessionFlesh& session_flesh, Storage& storage)
    {
        storage.reset();
        storage.begin_exec(request, session_flesh);
    }

}; // class executor

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
class storage;

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
struct iterator : std::iterator<
        std::input_iterator_tag, void, typename storage<
              Session, Entry, Container>::container_type ::difference_type, void, void
        >
{
    using self_type = iterator;

    using storage_type = storage<Session, Entry, Container>;

    storage_type& storage_;

public:

    iterator(storage_type& storage)
        : storage_{storage}
    {}

    self_type
    operator++() const
    {
        storage_.step_fwd();
        return *this;
    }

    self_type
    operator++(int) const
    {
        self_type _tmp = *this;
        this->operator++();
        return _tmp;
    }

    void
    operator()() const
    {
        in();
    }

    void
    in() const
    {
        storage_.exec();
    }

    std::size_t
    pos() const
    {
        return storage_.pos();
    }

}; // struct iterator

template<class Session,
         template<typename Signature> class Entry,
         template<typename Element, typename... Args> class Container>
class storage
{
    using self_type = storage;

public:

    using iterator_type = cb::iterator<Session, Entry, Container>;

    using session_type = Session;

    using session_flesh = typename session_type::flesh_type;

    using session_context = typename session_type::context_type;

    using session_wrapper = std::reference_wrapper<session_context const>;

    using request_type = typename session_type::request_type;

    using entry_type = Entry<void (request_type, session_wrapper, iterator_type)>;

    using container_type = Container<entry_type>;

    using size_type = typename container_type::size_type;

    static_assert (traits::HasConstIterator<container_type>::value
                   and traits::TryCbegin<container_type>::value
                   and traits::TryCend<container_type>::value
                   and traits::HasSizeType<container_type>::value
                   and traits::TrySize<container_type>::value
                   and traits::TryPushBack<container_type, entry_type>::value,
                   "Invalid a cb container!");

    static_assert (traits::HasRequestType<session_type>::value
                   and traits::HasContextType<session_type>::value
                   and traits::HasFleshType<session_type>::value,
                   "Invalid session type");

    static_assert (traits::TryInvoke<entry_type, request_type, session_wrapper, iterator_type>::value,
                   "Invalid entry type!");

    BEASTHTTP_DECLARE_FRIEND_BASE_CB_EXECUTOR_CLASS

    BEASTHTTP_DECLARE_FRIEND_CB_ITERATOR_STRUCT

    storage() = default;

    template<class Head, class... Tail,
             typename = typename std::enable_if<
                 not std::is_same<
                     typename std::decay<Head>::type, self_type
                     >::value
                 >::type
             >
    storage(Head&& head, Tail&&... tail)
        : container_{prepare(std::forward<Head>(head), std::forward<Tail>(tail)...)},
          it_next_{container_.cbegin()},
          request_{nullptr},
          session_flesh_{nullptr},
          cb_pos_{size_type{}}
    {}

private:

    template<class... OnRequest>
    container_type
    prepare(OnRequest&&... on_request)
    {
        container_type _l;

        const auto& tuple_cb = std::make_tuple(std::forward<OnRequest>(on_request)...);

        static_assert(std::tuple_size<typename std::decay<decltype (tuple_cb) >::type>::value != 0,
                      "Oops...! tuple is empty.");

#if not defined __cpp_generic_lambdas
        details::for_each<0>(tuple_cb,
                             details::cb_push_cxx11<container_type>{_l},
                             details::cb_push_fin_cxx11<container_type>{_l});
#else
        details::for_each<0>(tuple_cb,
                             [&_l](const auto& value){
            _l.push_back(
                        entry_type(
                            std::bind<void>(
                                value,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3)));},
                          [&_l](const auto & value){
            _l.push_back(
                        entry_type(
                            std::bind<void>(
                                value,
                                std::placeholders::_1,
                                std::placeholders::_2)));});
#endif
        return _l;
    }

    void
    step_fwd()
    {
        it_next_++; cb_pos_++;

        if (it_next_ == container_.cend()) {
            it_next_--; cb_pos_--;
        }

        skip_target();
    }

    void
    exec()
    {
        do_exec<self_type>{}(*this);
    }

    /**
      @brief Return actual current position cb in list
    */
    std::size_t
    pos()
    {
        return cb_pos_;
    }

    void
    begin_exec(request_type& request, session_flesh& session_flesh)
    {
        request_ = std::addressof(request);
        session_flesh_ = std::addressof(session_flesh);

        current_target_ = request_->target().to_string();

        if (container_.size() > 1)
            skip_target();

        do_exec<self_type>{}(*this);
    }

    template<typename _Self>
    struct do_exec
    {
        void
        operator()(_Self& self)
        {
            session_context _ctx{*self.session_flesh_};
            (*self.it_next_) (*self.request_, std::cref(_ctx), iterator_type{self});
        }
    }; // struct do_exec

    void
    reset()
    {
        it_next_ = container_.cbegin();
        cb_pos_ = 0;
    }

    void
    skip_target()
    {
        std::size_t pos = current_target_.find('/', 1);

        if (pos != std::string::npos) {
            auto next_target = current_target_.substr(0, pos);
            current_target_ = current_target_.substr(pos);

            request_->target(next_target);
        }
        else
            request_->target(current_target_);
    }

    container_type container_;
    typename container_type::const_iterator it_next_;
    request_type* request_;
    session_flesh* session_flesh_;
    std::string current_target_;
    size_type cb_pos_;

}; // class storage

} // namespace cb
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_CB_HXX
