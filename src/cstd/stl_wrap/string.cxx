module;
#include <cstdint>
#include <string>
export module cstd.stl_wrap.string;

export namespace cstd {

template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
using basic_string = std::basic_string<CharT, Traits, Alloc>;

using string = cstd::basic_string<char>;
using wstring = cstd::basic_string<wchar_t>;
using u8string = cstd::basic_string<char8_t>;
using u16string = cstd::basic_string<char16_t>;
using u32string = cstd::basic_string<char32_t>;

#define BASIC_STRING cstd::basic_string<CharT, Traits, Alloc>

template <typename CharT, typename Traits, typename Alloc>
constexpr bool operator==(const BASIC_STRING& x, const BASIC_STRING& y) noexcept {
  return std::operator==(x, y);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr auto operator<=>(const BASIC_STRING& x, const BASIC_STRING& y) noexcept {
  return std::operator<=>(x, y);
}

template <typename CharT, typename Traits, typename Alloc>
constexpr bool operator==(const BASIC_STRING& x, const CharT* y) {
  return std::operator==(x, y);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr auto operator<=>(const BASIC_STRING& x, const CharT* y) {
  return std::operator<=>(x, y);
}

// string <-> string. copy or move
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(const BASIC_STRING& lhs, const BASIC_STRING& rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(BASIC_STRING&& lhs, const BASIC_STRING& rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(const BASIC_STRING& lhs, BASIC_STRING&& rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(BASIC_STRING&& lhs, BASIC_STRING&& rhs) {
  return std::operator+(lhs, rhs);
}
// charT, const CharT* and copied string
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(const BASIC_STRING& lhs, CharT rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(const BASIC_STRING& lhs, const CharT* rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(CharT lhs, const BASIC_STRING& rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(const CharT* lhs, const BASIC_STRING& rhs) {
  return std::operator+(lhs, rhs);
}
// charT, const CharT* and moved string
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(BASIC_STRING&& lhs, CharT rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(BASIC_STRING&& lhs, const CharT* rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(CharT lhs, BASIC_STRING&& rhs) {
  return std::operator+(lhs, rhs);
}
template <typename CharT, typename Traits, typename Alloc>
constexpr BASIC_STRING operator+(const CharT* lhs, BASIC_STRING&& rhs) {
  return std::operator+(lhs, rhs);
}

template <typename CharT, typename Traits, typename Alloc>
void swap(BASIC_STRING& lhs, BASIC_STRING& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  std::swap(lhs, rhs);
}

int stoi(const cstd::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoi(str, pos, base);
}
int stoi(const cstd::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoi(str, pos, base);
}
long stol(const cstd::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stol(str, pos, base);
}
long stol(const cstd::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stol(str, pos, base);
}
long long stoll(const cstd::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoll(str, pos, base);
}
long long stoll(const cstd::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoll(str, pos, base);
}

unsigned long stoul(const cstd::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoul(str, pos, base);
}
unsigned long stoul(const cstd::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoul(str, pos, base);
}
unsigned long long stoull(const cstd::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoull(str, pos, base);
}
unsigned long long stoull(const cstd::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoull(str, pos, base);
}

float stof(const cstd::string& str, std::size_t* pos = nullptr) {
  return std::stof(str, pos);
}
float stof(const cstd::wstring& str, std::size_t* pos = nullptr) {
  return std::stof(str, pos);
}
double stod(const cstd::string& str, std::size_t* pos = nullptr) {
  return std::stod(str, pos);
}
double stod(const cstd::wstring& str, std::size_t* pos = nullptr) {
  return std::stod(str, pos);
}
long double stold(const cstd::string& str, std::size_t* pos = nullptr) {
  return std::stold(str, pos);
}
long double stold(const cstd::wstring& str, std::size_t* pos = nullptr) {
  return std::stold(str, pos);
}

cstd::string to_string(int value) {
  return std::to_string(value);
}
cstd::string to_string(long value) {
  return std::to_string(value);
}
cstd::string to_string(long long value) {
  return std::to_string(value);
}
cstd::string to_string(unsigned value) {
  return std::to_string(value);
}
cstd::string to_string(unsigned long value) {
  return std::to_string(value);
}
cstd::string to_string(unsigned long long value) {
  return std::to_string(value);
}
cstd::string to_string(float value) {
  return std::to_string(value);
}
cstd::string to_string(double value) {
  return std::to_string(value);
}
cstd::string to_string(long double value) {
  return std::to_string(value);
}

cstd::wstring to_wstring(int value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(long value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(long long value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(unsigned value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(unsigned long value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(unsigned long long value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(float value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(double value) {
  return std::to_wstring(value);
}
cstd::wstring to_wstring(long double value) {
  return std::to_wstring(value);
}

// cstd::pmr
namespace pmr {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string = cstd::basic_string<CharT, Traits, std::pmr::polymorphic_allocator<CharT>>;

using string = cstd::pmr::basic_string<char>;
using wstring = cstd::pmr::basic_string<wchar_t>;
using u8string = cstd::pmr::basic_string<char8_t>;
using u16string = cstd::pmr::basic_string<char16_t>;
using u32string = cstd::pmr::basic_string<char32_t>;

} // namespace pmr

// cstd::string_literals
inline namespace string_literals {
constexpr inline cstd::string operator""_s(const char* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline cstd::wstring operator""_s(const wchar_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline cstd::u8string operator""_s(const char8_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline cstd::u16string operator""_s(const char16_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline cstd::u32string operator""_s(const char32_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}

} // namespace string_literals

} // namespace cstd
