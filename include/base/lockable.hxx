#if not defined BEASTHTTP_BASE_LOCKABLE_HXX
#define BEASTHTTP_BASE_LOCKABLE_HXX

#if __cplusplus >= 201703L
#define BEASTHTTP_CXX17_SHARED_MUTEX
#include <mutex>
#include <shared_mutex>
#else
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/lock_factories.hpp>
#endif

namespace _0xdead4ead {
namespace http {
namespace base {

struct lockable
{
#if defined BEASTHTTP_CXX17_SHARED_MUTEX
    using mutex_type = std::shared_mutex;

    using shared_lock_type = std::shared_lock<mutex_type>;

    using unique_lock_type = std::unique_lock<mutex_type>;
#else
    using mutex_type = boost::shared_mutex;

    using shared_lock_type = boost::shared_lock<mutex_type>;

    using unique_lock_type = boost::unique_lock<mutex_type>;
#endif

    static shared_lock_type
    enter_to_read(mutex_type& mutex)
    {
        return shared_lock_type(mutex);
    }

    static unique_lock_type
    enter_to_write(mutex_type& mutex)
    {
        return unique_lock_type(mutex);
    }

#if defined BEASTHTTP_CXX17_SHARED_MUTEX
    static auto
    enter_to_write(mutex_type& mutex_1, mutex_type& mutex_2)
    {
        return std::scoped_lock(mutex_1, mutex_2);
    }
#else
    static std::tuple<unique_lock_type, unique_lock_type>
    enter_to_write(mutex_type& mutex_1, mutex_type& mutex_2)
    {
        return boost::make_unique_locks(mutex_1, mutex_2);
    }
#endif
}; // class lockable

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_LOCKABLE_HXX
