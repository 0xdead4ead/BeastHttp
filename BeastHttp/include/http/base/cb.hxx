#if not defined BEASTHTTP_BASE_CB_HXX
#define BEASTHTTP_BASE_CB_HXX

#include <http/base/traits.hxx>

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
class storage;

BEASTHTTP_DECLARE_STORAGE_TEMPLATE
struct iterator : std::iterator<
        std::input_iterator_tag, void, typename storage<
              Session, Entry, Container>::container_type ::difference_type,
        void, void
        >
{
    using self_type = iterator;

    using storage_type = storage<Session, Entry, Container>;

    storage_type& storage_;

public:

    iterator(storage_type&) noexcept;

    inline self_type
    operator++() const;

    inline self_type
    operator++(int) const;

    inline void
    operator()() const;

    inline void
    in() const;

    inline std::size_t
    pos() const;

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

    using request_type = typename session_type::request_type;

    using entry_type = Entry<void (request_type, session_context, iterator_type)>;

    using container_type = Container<entry_type>;

    using size_type = typename container_type::size_type;

    static_assert (traits::HasConstIterator<container_type, void>::value
                   and traits::TryCbegin<container_type, void()>::value
                   and traits::TryCend<container_type, void()>::value
                   and traits::HasSizeType<container_type, void>::value
                   and traits::TrySize<container_type, void()>::value
                   and traits::TryPushBack<container_type, void(entry_type)>::value,
                   "Invalid a cb container!");

    static_assert (traits::HasRequestType<session_type, void>::value
                   and traits::HasContextType<session_type, void>::value
                   and traits::HasFleshType<session_type, void>::value,
                   "Invalid session type");

    static_assert (traits::TryInvoke<entry_type, void(request_type, session_context, iterator_type)>::value,
                   "Invalid entry type!");

    BEASTHTTP_DECLARE_FRIEND_BASE_CB_EXECUTOR_CLASS

    BEASTHTTP_DECLARE_FRIEND_CB_ITERATOR_STRUCT

    storage() = default;

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

    void
    step_fwd();

    void
    exec();

    std::size_t
    pos();

    void
    begin_exec(request_type&, session_flesh&);

    template<typename _Self>
    struct do_exec
    {
        void
        operator()(_Self&);
    };

    void
    reset();

    void
    skip_target();

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

#include <http/base/impl/cb.hxx>

#endif // not defined BEASTHTTP_BASE_CB_HXX
