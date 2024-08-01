module;
#include <cstdint>
#include <string_view>
export module csc.stl_wrap.string_view;

export namespace csc {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view : public std::basic_string_view<CharT, Traits> {};

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u8string_view = basic_string_view<char8_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

template <typename CharT, typename Traits>
constexpr bool operator==(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept {
  return x == y;
}

template <typename CharT, typename Traits>
constexpr auto operator<=>(basic_string_view<CharT, Traits> x, basic_string_view<CharT, Traits> y) noexcept {
  return x <=> y;
}
} // namespace csc