module;
#include <cstdint>
#include <string>
export module stl.stl_wrap.string;

export namespace std {

using std::basic_string;
using std::string;
using std::wstring;
using std::u8string;
using std::u16string;
using std::u32string;

using std::char_traits;

using std::operator==;
using std::operator<=>;
using std::operator+;

using std::swap;

using std::stoi;
using std::stol;
using std::stoll;
using std::stoul;
using std::stoull;
using std::stof;
using std::stod;
using std::stold;

using std::to_string;
using std::to_wstring;

// std::pmr
namespace pmr {
using std::pmr::string;
} // namespace pmr

inline namespace string_literals {
using std::string_literals::operator""s;
} // namespace string_literals

} // namespace stl
