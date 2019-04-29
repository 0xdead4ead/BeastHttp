#if not defined BEASTHTTP_BASE_IMPL_REGEX_HXX
#define BEASTHTTP_BASE_IMPL_REGEX_HXX

namespace _0xdead4ead {
namespace http {
namespace base {

regex::regex(flag_type flags)
    : flags_{flags}
{
}

bool
regex::match(const std::string& regx, const std::string& str)
{
    const regex_type e(regx, flags_);
    return boost::regex_match(str, e);
}

bool
regex::match(const std::string& regx, const std::string& str,
      boost::smatch& results)
{
    const regex_type e(regx, flags_);
    return boost::regex_match(str, results, e);
}

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_IMPL_REGEX_HXX
