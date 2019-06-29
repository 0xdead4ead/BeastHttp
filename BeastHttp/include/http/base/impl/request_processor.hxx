#if not defined BEASTHTTP_BASE_IMPL_REQUEST_PROCESSOR_HXX
#define BEASTHTTP_BASE_IMPL_REQUEST_PROCESSOR_HXX

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Session>
request_processor<Session>::request_processor(
        std::shared_ptr<resource_map_type> const& resource_map,
        std::shared_ptr<method_map_type> const& method_map,
        typename regex_type::flag_type flags)
    : resource_map_{resource_map},
      method_map_{method_map},
      regex_{flags}
{
}

#if defined BEASTHTTP_TEST_ROUTER
template<class Session>
void
request_processor<Session>::provide(
        request_type&& request, session_flesh&& _flesh)
{
    provide(request, _flesh);
}
#endif // defined BEASTHTTP_TEST_ROUTER

template<class Session>
void
request_processor<Session>::provide(
        request_type& request, session_flesh& _flesh)
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

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_REQUEST_PROCESSOR_HXX
