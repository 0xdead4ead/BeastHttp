#if not defined BEASTHTTP_BASE_IMPL_CB_HXX
#define BEASTHTTP_BASE_IMPL_CB_HXX

#include <http/base/config.hxx>

#include <functional>

namespace _0xdead4ead {
namespace http {
namespace base {
namespace cb {
namespace detail {
#if not defined BEASTHTTP_CXX17_IF_CONSTEXPR
template<std::size_t value>
using size_type = std::integral_constant<std::size_t, value>;

template <class Begin, class End, typename S, class... Elements>
struct for_each_impl
{
    void
    operator()(const std::tuple<Elements...>& tpl,
               const Begin& begin,
               const End& end)
    {
        const auto& value = std::get<S::value>(tpl);
        begin(value);
        for_each_impl<Begin, End, size_type<S() + 1>,
                Elements...>{}(tpl, begin, end);
    }

}; // struct for_each_impl

template <class Begin, class End, class... Elements>
struct for_each_impl<Begin, End,
        size_type<std::tuple_size<std::tuple<Elements...>>::value - 1>,
        Elements...>
{
    void
    operator()(const std::tuple<Elements...>& tpl,
               const Begin& begin,
               const End& end)
    {
        const auto& value =
                std::get<size_type<std::tuple_size<
                std::tuple<Elements...>>::value - 1>::value>(tpl);
        end(value);
        (void)begin;
    }

}; // struct for_each_impl

template<std::size_t Index, class Begin, class End, class... Elements>
void
for_each(const std::tuple<Elements...>& tpl,
         const Begin& begin,
         const End& end)
{
    for_each_impl<Begin, End, size_type<Index>, Elements...>{}(tpl, begin, end);
}
#else
template<std::size_t Index, class Begin, class End, class... Elements>
void
for_each(const std::tuple<Elements...>& tpl,
         const Begin& begin,
         const End& end)
{
    const auto& value = std::get<Index>(tpl);
    if constexpr (Index + 1 == std::tuple_size<std::tuple<Elements...>>::value)
        end(value);
    else {
        begin(value);
        for_each<Index + 1, Begin, End, Elements...>(tpl, begin, end);
    }
}
#endif // BEASTHTTP_CXX17_IF_CONSTEXPR

#if not defined BEASTHTTP_CXX14_GENERIC_LAMBDAS
template<class Container>
struct cb_push_cxx11
{
    using container_type = Container;

    using value_type = typename container_type::value_type;

    Container& l_;

    cb_push_cxx11(Container& l)
        : l_{l}
    {
    }

    template<class F>
    void
    operator()(const F& value) const
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

    cb_push_fin_cxx11(Container& l)
        : l_{l}
    {
    }

    template<class F>
    void
    operator()(const F& value) const
    {
        l_.push_back(
                    value_type(
                        std::bind<void>(
                            value,
                            std::placeholders::_1,
                            std::placeholders::_2)));
    }
}; // struct cb_push_fin_cxx11

#endif // BEASTHTTP_CXX14_GENERIC_LAMBDAS
} // namespace detail

template<class Request, class SessionFlesh, class Storage>
void
executor::execute(Request& request,
                  SessionFlesh& session_flesh,
                  Storage& storage)
{
    storage.begin_exec(request, session_flesh)();
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
const_iterator<Session, Entry, Container>::const_iterator(
        const container_type& container, request_type& request,
        session_flesh& flesh)
    : pos_{0},
      cont_begin_iter_{container.begin()},
      cont_end_iter_{container.end()},
      request_{request},
      session_flesh_{flesh},
      current_target_{request_.target().to_string()}
{
    if (container.size() > 1)
        skip_target();
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
typename const_iterator<Session, Entry, Container>::self_type&
const_iterator<Session, Entry, Container>::operator++()
{
    cont_begin_iter_++; pos_++;

    if (cont_begin_iter_ == cont_end_iter_) {
        cont_begin_iter_--; pos_--;
    }

    skip_target();

    return *this;
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
typename const_iterator<Session, Entry, Container>::self_type
const_iterator<Session, Entry, Container>::operator++(int)
{
    self_type _tmp{*this};
    ++(*this);
    return _tmp;
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
void
const_iterator<Session, Entry, Container>::operator()() const
{
    session_context _ctx{session_flesh_};
    (*cont_begin_iter_) (request_, std::move(_ctx), *this);
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
void
const_iterator<Session, Entry, Container>::skip_target()
{
    std::size_t pos = current_target_.find('/', 1);

    if (pos != std::string::npos) {
        auto next_target = current_target_.substr(0, pos);
        current_target_ = current_target_.substr(pos);

        request_.target(next_target);
    }
    else
        request_.target(current_target_);
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
typename const_iterator<Session, Entry, Container>::size_type
const_iterator<Session, Entry, Container>::pos() const
{
    return pos_;
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
template<class F, class... Fn, typename>
storage<Session, Entry, Container>::storage(F&& f, Fn&&... fn)
    : container_{prepare(std::forward<F>(f),
                         std::forward<Fn>(fn)...)}
{
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
template<class... OnRequest>
typename storage<Session, Entry, Container>::container_type
storage<Session, Entry, Container>::prepare(OnRequest&&... on_request)
{
    container_type _l;

    const auto& tuple_cb = std::make_tuple(
                std::forward<OnRequest>(on_request)...);

    static_assert(std::tuple_size<typename std::decay<
                  decltype (tuple_cb) >::type>::value != 0,
                  "Oops...! tuple is empty.");

#if not defined BEASTHTTP_CXX14_GENERIC_LAMBDAS
    detail::for_each<0>(tuple_cb,
                        detail::cb_push_cxx11<container_type>{_l},
                        detail::cb_push_fin_cxx11<container_type>{_l});
#else
    detail::for_each<0>(tuple_cb,
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
#endif // BEASTHTTP_CXX14_GENERIC_LAMBDAS
    return _l;
}

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
typename storage<Session, Entry, Container>::iterator_type
storage<Session, Entry, Container>::begin_exec(
        request_type& request, session_flesh& flesh)
{
    return iterator_type(container_, request, flesh);
}

} // namespace cb
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_CB_HXX
