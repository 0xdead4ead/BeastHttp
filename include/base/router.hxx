#if not defined BEASTHTTP_BASE_ROUTER_HXX
#define BEASTHTTP_BASE_ROUTER_HXX

#include <base/traits.hxx>

#include <memory>

namespace _0xdead4ead {
namespace http {
namespace base {

#define BEASTHTTP_PACK_PROVIDE_CALL(r, f) \
    Pack::template provide<DerivedRouter>(r, f)

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

    using request_type = typename session_type::request_type;

    static_assert (base::traits::HasStorageType<session_type, void>::value
                   and base::traits::HasResourceMapType<session_type, void>::value
                   and base::traits::HasMethodMapType<session_type, void>::value
                   and base::traits::HasResourceRegexType<session_type, void>::value
                   and base::traits::HasResourceType<session_type, void>::value
                   and base::traits::HasMethodType<session_type, void>::value
                   and base::traits::HasRegexType<session_type, void>::value
                   and base::traits::HasRequestType<session_type, void>::value,
                   "Invalid session type!");

    explicit
    router(std::shared_ptr<resource_map_type>&,
           std::shared_ptr<method_map_type>&) noexcept;

    void
    add_resource_cb(resource_regex_type const&,
                    method_type const&,
                    storage_type&&);

    void
    add_resource_cb_without_method(resource_regex_type const&,
                                   storage_type&&);

    void
    use(resource_regex_type const&,
        self_type const&);

public:

    std::shared_ptr<resource_map_type> const&
    resource_map() const;

    std::shared_ptr<method_map_type> const&
    method_map() const;

    template<class DerivedRouter, class Pack>
    auto
    param(DerivedRouter& router, typename regex_type::flag_type flags)
    -> decltype (BEASTHTTP_PACK_PROVIDE_CALL(router, flags));

private:

    resource_regex_type
    concat(const resource_regex_type&, const resource_regex_type&);

    std::shared_ptr<resource_map_type>& resource_map_;
    std::shared_ptr<method_map_type>& method_map_;

}; // class router

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/router.ixx>

#endif // not defined BEASTHTTP_BASE_ROUTER_HXX
