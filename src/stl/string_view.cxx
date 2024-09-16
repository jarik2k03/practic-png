module;
#include <cstdint>
#include <string_view>
export module stl.string_view;

export namespace std {

using std::basic_string_view;

using std::string_view;
using std::u16string_view;
using std::u32string_view;
using std::u8string_view;
using std::wstring_view;

using std::operator==;
using std::operator<=>;

inline namespace string_view_literals {
using std::string_view_literals::operator""sv;
}

} // namespace std
