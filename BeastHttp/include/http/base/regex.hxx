#if not defined BEASTHTTP_BASE_REGEX_HXX
#define BEASTHTTP_BASE_REGEX_HXX

#include <regex>

namespace _0xdead4ead {
namespace http {
namespace base {

class regex
{
    using self_type = regex;

public:

    using char_type = char;

    using traits_type = std::regex_traits<char_type>;

    using regex_type = std::basic_regex<char_type, traits_type>;

    using flag_type = typename regex_type::flag_type;

    inline regex(flag_type);

    inline bool
    match(const std::string&, const std::string&);

    inline bool
    match(const std::string&, const std::string&, std::smatch&);

private:

    flag_type flags_;

}; // class regex

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <http/base/impl/regex.hxx>

#endif // not defined BEASTHTTP_BASE_REGEX_HXX
