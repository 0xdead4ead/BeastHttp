#if not defined BEASTHTTP_BASE_QUEUE_IXX
#define BEASTHTTP_BASE_QUEUE_IXX

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Flesh>
queue<Flesh>::queue(Flesh& impl)
    : impl_(impl)
{
    static_assert(limit > 0, "queue limit must be positive");
    items_.reserve(limit);
}

template<class Flesh>
bool
queue<Flesh>::is_full() const
{
    return items_.size() >= limit;
}

template<class Flesh>
bool
queue<Flesh>::on_write()
{
    assert(not items_.empty());
    auto const was_full = is_full();
    items_.erase(items_.begin());
    if (not items_.empty())
        (*items_.front())();
    return was_full;
}

template<class Flesh>
template<class Response>
void
queue<Flesh>::operator()(Response&& response)
{
    struct work_impl : work
    {
        Flesh& impl_;
        typename std::decay<Response>::type response_;

        work_impl(Flesh& impl, Response&& response)
            : impl_(impl)
            , response_(std::forward<Response>(response))
        {}

        void
        operator()()
        {
            impl_.do_write(response_);
        }
    };
#if not defined __cpp_lib_make_unique
    items_.push_back(std::unique_ptr<work_impl>(
                         new work_impl(impl_,
                                       std::forward<Response>(response))));
#else
    items_.push_back(std::make_unique<work_impl>(
                         impl_, std::forward<Response>(response)));
#endif
    if (items_.size() == 1)
        (*items_.front())();
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_QUEUE_IXX
