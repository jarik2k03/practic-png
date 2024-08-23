module;
#include <cstdint>
#include <string_view>
export module cstd.stl_wrap.string_view;

export namespace cstd {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string_view = std::basic_string_view<CharT, Traits>;

using string_view = cstd::basic_string_view<char>;
using wstring_view = cstd::basic_string_view<wchar_t>;
using u8string_view = cstd::basic_string_view<char8_t>;
using u16string_view = cstd::basic_string_view<char16_t>;
using u32string_view = cstd::basic_string_view<char32_t>;

template <typename CharT, typename Traits>
constexpr bool operator==(cstd::basic_string_view<CharT, Traits> x, cstd::basic_string_view<CharT, Traits> y) noexcept {
  return std::operator==(x, y);
}

template <typename CharT, typename Traits>
constexpr auto operator<=>(
    cstd::basic_string_view<CharT, Traits> x,
    cstd::basic_string_view<CharT, Traits> y) noexcept {
  return std::operator<=>(x, y);
}
} // namespace cstd