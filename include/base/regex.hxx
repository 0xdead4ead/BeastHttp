#if not defined BEASTHTTP_BASE_REGEX_HXX
#define BEASTHTTP_BASE_REGEX_HXX

#include "traits.hxx"

#include <string>
#include <boost/regex.hpp>

namespace _0xdead4ead {
namespace http {
namespace base {

class regex
{
    using self_type = regex;

public:

    using char_type = char;

    using traits_type = boost::regex_traits<char_type>;

    using regex_type = boost::basic_regex<char_type, traits_type>;

    using flag_type = typename regex_type::flag_type;

    regex(flag_type flags)
        : flags_{flags}
    {}

    bool
    match(const std::string& regx, const std::string& str)
    {
        const regex_type e(regx, flags_);
        return boost::regex_match(str, e);
    }

    bool
    match(const std::string& regx, const std::string& str,
          boost::smatch& results)
    {
        const regex_type e(regx, flags_);
        return boost::regex_match(str, results, e);
    }

private:

    flag_type flags_;

}; // class regex

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#endif // not defined BEASTHTTP_BASE_REGEX_HXX
