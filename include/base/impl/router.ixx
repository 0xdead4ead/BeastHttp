#if not defined BEASTHTTP_BASE_ROUTER_IXX
#define BEASTHTTP_BASE_ROUTER_IXX

namespace _0xdead4ead {
namespace http {
namespace base {

template<class Session>
router<Session>::router(std::shared_ptr<resource_map_type>& resource_map,
                        std::shared_ptr<method_map_type>& method_map) noexcept
    : resource_map_{resource_map},
      method_map_{method_map}
{
}

template<class Session>
void
router<Session>::add_resource_cb(
        resource_regex_type const& path_to_resource,
        method_type const& method,
        storage_type&& storage)
{
    if (path_to_resource.empty()) // can not place callback with empty regex
        return;

    if (not method_map_)
        method_map_ = std::make_shared<method_map_type>();

    method_map_->insert({
                            method,
                            resource_map_type()
                        });

    auto& resource_map_ref = method_map_->at(method);

    resource_map_ref.insert({
                                path_to_resource,
                                std::shared_ptr<storage_type>{}
                            });

    auto& storage_sp = resource_map_ref.at(path_to_resource);
    if (not storage_sp)
        storage_sp = std::make_shared<storage_type>();

    *storage_sp = std::move(storage);
}

template<class Session>
void
router<Session>::add_resource_cb_without_method(
        resource_regex_type const& path_to_resource,
        storage_type&& storage)
{
    if (path_to_resource.empty()) // can not place callback with empty regex
        return;

    if (not resource_map_)
        resource_map_ = std::make_shared<resource_map_type>();

    resource_map_->insert({
                              path_to_resource,
                              std::shared_ptr<storage_type>{}
                          });

    auto & storage_sp = resource_map_->at(path_to_resource);
    if (not storage_sp)
        storage_sp = std::make_shared<storage_type>();

    *storage_sp = std::move(storage);
}

template<class Session>
void
router<Session>::use(resource_regex_type const& path_to_resource,
                     self_type const& other)
{
    if (other.resource_map_)
        for (const auto& value : *other.resource_map_) {
            auto storage = *value.second;
            add_resource_cb_without_method(
                        concat(path_to_resource, value.first),
                        std::move(storage));
        }

    if (other.method_map_)
        for (const auto& value_m : *other.method_map_) {
            auto method = value_m.first;
            const auto& resource_map = value_m.second;

            for (const auto& value_r : resource_map) {
                auto storage = *value_r.second;
                add_resource_cb(concat(path_to_resource, value_r.first),
                                method, std::move(storage));
            }
        }
}

template<class Session>
template<class DerivedRouter, class Pack>
auto
router<Session>::param(DerivedRouter& router,
                       typename regex_type::flag_type flags)
-> decltype (BEASTHTTP_PACK_PROVIDE_CALL(router, flags))
{
    return BEASTHTTP_PACK_PROVIDE_CALL(router, flags);
}

template<class Session>
std::shared_ptr<typename router<Session>::resource_map_type> const&
router<Session>::resource_map() const
{
    return resource_map_;
}

template<class Session>
std::shared_ptr<typename router<Session>::method_map_type> const&
router<Session>::method_map() const
{
    return method_map_;
}

template<class Session>
typename router<Session>::resource_regex_type
router<Session>::concat(const resource_regex_type& resource1,
                        const resource_regex_type& resource2)
{
    resource_regex_type result;
    if (resource1.back() == '$' and resource1.front() == '^' and
            resource2.back() == '$' and resource2.front() == '^')
        result = resource1.substr(0, resource1.size() - 1) + resource2.substr(1);
    else if (resource1.back() == '$' and resource1.front() == '^')
        result = resource1.substr(0, resource1.size() - 1) + resource2 + "$";
    else if (resource2.back() == '$' and resource2.front() == '^')
        result = "^" + resource1 + resource2.substr(1);
    else
        result = resource1 + resource2;

    return result;
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_ROUTER_IXX
