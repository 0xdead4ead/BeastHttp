#if not defined BEASTHTTP_BASE_REGEX_HXX
#define BEASTHTTP_BASE_REGEX_HXX

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

    regex(flag_type);

    bool
    match(const std::string&, const std::string&);

    bool
    match(const std::string&, const std::string&, boost::smatch&);

private:

    flag_type flags_;

}; // class regex

} // namespace base
} // namespace http
} // namespace _0xdead4ead

#include <base/impl/regex.hxx>

#endif // not defined BEASTHTTP_BASE_REGEX_HXX
