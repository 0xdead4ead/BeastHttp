#if not defined BEASTHTTP_BASE_REQUEST_PROCESSOR_HXX
#define BEASTHTTP_BASE_REQUEST_PROCESSOR_HXX

#include "traits.hxx"

#include <boost/beast/http/message.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Session>
class request_processor : private Session::cbexecutor_type
{
    using session_type = Session;

    static_assert (traits::HasCbExecutorType<session_type>::value
                   and traits::HasResourceMapType<session_type>::value
                   and traits::HasMethodMapType<session_type>::value
                   and traits::HasResourceType<session_type>::value
                   and traits::HasMethodType<session_type>::value
                   and traits::HasFleshType<session_type>::value
                   and traits::HasRegexType<session_type>::value
                   and traits::HasRequestType<session_type>::value,
                   "Invalid session type!");

public:

    using resource_map_type = typename session_type::resource_map_type;

    using method_map_type = typename session_type::method_map_type;

    using resource_type = typename session_type::resource_type;

    using method_type = typename session_type::method_type;

    using session_flesh = typename session_type::flesh_type;

    using regex_type = typename session_type::regex_type;

    using request_type = typename session_type::request_type;

    static_assert (traits::TryCbegin<resource_map_type>::value
                   and traits::TryCend<resource_map_type>::value,
                   "Invalid resource container!");

    static_assert (traits::TryCbegin<method_map_type>::value
                   and traits::TryCend<method_map_type>::value
                   and traits::TryFind<method_map_type, method_type>::value,
                   "Invalid method/verb container!");

    request_processor(std::shared_ptr<resource_map_type> const& resource_map,
                      std::shared_ptr<method_map_type> const& method_map,
                      typename regex_type::flag_type flags)
        : resource_map_{resource_map},
          method_map_{method_map},
          regex_{flags}
    {}

    /**
      Tests only...
    */
    void
    provide(request_type&& request, session_flesh&& _flesh)
    {
        provide(request, _flesh);
    }

    void
    provide(request_type& request, session_flesh& _flesh)
    {
        resource_type target = request.target();
        method_type method = request.method();

        bool invoked = false;
        if (method_map_) {
            auto method_pos = static_cast<method_map_type const &>(*method_map_).find(method);
            if (method_pos != method_map_->cend()) {

                auto& resource_map = method_pos->second;

                for (auto __it_value = resource_map.cbegin();
                     __it_value != resource_map.cend(); ++__it_value) {
                    if (regex_.match(__it_value->first, target.to_string())) {
                        auto const& storage = __it_value->second;

                        if (storage) {
                            this->execute(request, _flesh, *storage);
                            invoked = true;
                        }
                    }
                }
            }
        }

        if (resource_map_ and not invoked)
            for (auto __it_value = resource_map_->cbegin();
                 __it_value != resource_map_->cend(); ++__it_value) {
                if (regex_.match(__it_value->first, target.to_string())) {
                    auto const& storage = __it_value->second;

                    if (storage)
                        this->execute(request, _flesh, *storage);
                }
            }
    }

private:

    std::shared_ptr<resource_map_type> const& resource_map_;
    std::shared_ptr<method_map_type> const& method_map_;

    regex_type regex_;

}; // request_processor

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // BEASTHTTP_BASE_REQUEST_PROCESSOR_HXX
