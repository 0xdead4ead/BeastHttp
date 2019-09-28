#if not defined BEASTHTTP_BASE_IMPL_QUEUE_HXX
#define BEASTHTTP_BASE_IMPL_QUEUE_HXX

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
queue<Flesh>::operator()(Response& response)
{
    using response_type = typename std::decay<Response>::type;

    struct work_impl : work
    {
        Flesh& impl_;
        response_type response_;

        work_impl(Flesh& impl, response_type&& response)
            : impl_(impl)
            , response_(std::move(response))
        {
        }

        void
        operator()()
        {
            impl_.do_write(response_);
        }
    };
#if not defined __cpp_lib_make_unique
    items_.push_back(std::unique_ptr<work_impl>(
                         new work_impl(impl_, std::move(response))));
#else
    items_.push_back(std::make_unique<work_impl>(
                         impl_, std::move(response)));
#endif
    if (items_.size() == 1)
        (*items_.front())();
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_QUEUE_HXX
