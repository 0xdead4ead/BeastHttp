#if not defined BEASTHTTP_BASE_CB_HXX
#define BEASTHTTP_BASE_CB_HXX

#include <http/base/traits.hxx>

#include <string>
#include <iterator>

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

class executor
{
protected:

    template<class Request, class SessionFlesh, class Storage>
    void
    execute(Request&, SessionFlesh&, Storage&);

}; // class executor

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
struct const_iterator
{
    using self_type = const_iterator;

    template<class, template<typename> class, template<typename, typename...> class>
    friend class storage;

    using session_type = Session;

    using session_flesh = typename session_type::flesh_type;

    using session_context = typename session_type::context_type;

    using request_type = typename session_type::request_type;

    using entry_type = Entry<void (request_type, session_context, self_type)>;

    using container_type = Container<entry_type>;

    using container_iterator = typename container_type::const_iterator;

    using size_type = typename container_type::size_type;

    // iterator_traits
    using iterator_category = std::input_iterator_tag;

    using value_type = typename container_iterator::value_type;

    using difference_type = typename container_iterator::difference_type;

    using pointer = typename container_iterator::pointer;

    using reference = typename container_iterator::reference;
    //

public:

    const_iterator(const container_type&, request_type&, session_flesh&);

    self_type&
    operator++();

    self_type
    operator++(int);

    void
    operator()() const;

    inline size_type
    pos() const;

private:
    void
    skip_target();

    size_type pos_;
    container_iterator cont_begin_iter_;
    container_iterator cont_end_iter_;
    request_type& request_;
    session_flesh& session_flesh_;
    std::string current_target_;

}; // struct iterator

template<class Session,
         template<typename Signature> class Entry,
         template<typename Element, typename... Args> class Container>
class storage
{
    using self_type = storage;

public:

    using session_type = Session;

    using session_flesh = typename session_type::flesh_type;

    using session_context = typename session_type::context_type;

    using request_type = typename session_type::request_type;

    using iterator_type = cb::const_iterator<Session, Entry, Container>;

    using entry_type = Entry<void (request_type, session_context, iterator_type)>;

    using container_type = Container<entry_type>;

    static_assert (traits::HasRequestType<session_type, void>::value
                   and traits::HasContextType<session_type, void>::value
                   and traits::HasFleshType<session_type, void>::value,
                   "Invalid session type");

    BEASTHTTP_DECLARE_FRIEND_BASE_CB_EXECUTOR_CLASS

    storage() = delete;

    template<class F, class... Fn,
             typename = typename std::enable_if<
                 not std::is_same<typename std::decay<F>::type, self_type>::value
                 and traits::TryInvokeConjunction<
                     sizeof... (Fn), void (request_type, session_context, iterator_type),
                     void (request_type, session_context), F, Fn...>::value
                 >::type
             >
    storage(F&&, Fn&&...);

private:

    template<class... OnRequest>
    container_type
    prepare(OnRequest&&...);

    iterator_type
    begin_exec(request_type&, session_flesh&);

    container_type container_;

}; // class storage

} // namespace cb
} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <http/base/impl/cb.hxx>

#endif // not defined BEASTHTTP_BASE_CB_HXX
