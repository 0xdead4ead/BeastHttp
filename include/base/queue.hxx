#if not defined BEASTHTTP_BASE_QUEUE_HXX
#define BEASTHTTP_BASE_QUEUE_HXX

#include <vector>
#include <memory>

namespace _0xdead4ead {
namespace http {
namespace base {

//https://www.boost.org/doc/libs/1_68_0/libs/beast/example/advanced/server/advanced_server.cpp
template<class Flesh>
class queue
{
    constexpr static size_t limit = 16;

    struct work
    {
        virtual ~work() = default;
        virtual void operator()() = 0;
    };

    Flesh& impl_;
    std::vector<std::unique_ptr<work>> items_;

public:

    explicit
    queue(Flesh&);

    bool
    is_full() const;

    bool
    on_write();

    template<class Response>
    void
    operator()(Response&&);

}; // class queue

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/queue.ixx>

#endif // not defined BEASTHTTP_BASE_QUEUE_HXX
