#if not defined BEASTHTTP_BASE_ROUTER_HXX
#define BEASTHTTP_BASE_ROUTER_HXX

#include "base/traits.hxx"
#include "base/cb.hxx"

#include <memory>

namespace http {
namespace base {

/**
  @brief Routing module class
*/
template<class Session>
class router
{
    using self_type = router;

protected:

    using session_type = Session;

    using storage_type = typename session_type::storage_type;

    using resource_map_type = typename session_type::resource_map_type;

    using method_map_type = typename session_type::method_map_type;

    using resource_regex_type = typename session_type::resource_regex_type;

    using resource_type = typename session_type::resource_type;

    using method_type = typename session_type::method_type;

    using regex_type = typename session_type::regex_type;

    static_assert (base::traits::HasStorageType<session_type>::value
                   and base::traits::HasResourceMapType<session_type>::value
                   and base::traits::HasMethodMapType<session_type>::value
                   and base::traits::HasResourceRegexType<session_type>::value
                   and base::traits::HasResourceType<session_type>::value
                   and base::traits::HasMethodType<session_type>::value
                   and base::traits::HasRegexType<session_type>::value,
                   "Invalid session type!");

    explicit
    router(std::shared_ptr<resource_map_type>& resource_map,
           std::shared_ptr<method_map_type>& method_map) noexcept
        : resource_map_{resource_map},
          method_map_{method_map}
    {}

    void
    add_resource_cb(resource_regex_type const& path_to_resource,
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

    void
    add_resource_cb_without_method(resource_regex_type const& path_to_resource,
                                   storage_type&& storage){

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

    void
    use(resource_regex_type const& path_to_resource,
        self_type const& other)
    {
        if (other.resource_map_)
            for (const auto& value : *other.resource_map_) {
                auto storage = *value.second;
                add_resource_cb_without_method(path_to_resource + value.first,
                                               std::move(storage));
            }

        if (other.method_map_)
            for (const auto& value_m : *other.method_map_) {
                auto method = value_m.first;
                const auto& resource_map = value_m.second;

                for (const auto& value_r : resource_map) {
                    auto storage = *value_r.second;
                    add_resource_cb(path_to_resource + value_r.first, method, std::move(storage));
                }
            }
    }

    template<class DerivedRouter, class Pack>
    auto
    param(DerivedRouter& router, typename regex_type::flag_type flags)
    {
        return Pack::template provide<DerivedRouter>(router, flags);
    }

public:

    std::shared_ptr<resource_map_type> const&
    resource_map() const
    {
        return resource_map_;
    }

    std::shared_ptr<method_map_type> const&
    method_map() const
    {
        return method_map_;
    }

private:

    std::shared_ptr<resource_map_type>& resource_map_;
    std::shared_ptr<method_map_type>& method_map_;
}; // class router

} // namespace base
} // namespace http

#endif // not defined BEASTHTTP_BASE_ROUTER_HXX
