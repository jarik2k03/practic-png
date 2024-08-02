module;
#include <cstdint>
#include <string_view>
export module csc.stl_wrap.string_view;

export namespace csc {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string_view = std::basic_string_view<CharT, Traits>;

using string_view = csc::basic_string_view<char>;
using wstring_view = csc::basic_string_view<wchar_t>;
using u8string_view = csc::basic_string_view<char8_t>;
using u16string_view = csc::basic_string_view<char16_t>;
using u32string_view = csc::basic_string_view<char32_t>;

template <typename CharT, typename Traits>
constexpr bool operator==(csc::basic_string_view<CharT, Traits> x, csc::basic_string_view<CharT, Traits> y) noexcept {
  return std::operator==(x, y);
}

template <typename CharT, typename Traits>
constexpr auto operator<=>(csc::basic_string_view<CharT, Traits> x, csc::basic_string_view<CharT, Traits> y) noexcept {
  return std::operator<=>(x, y);
}
} // namespace csc