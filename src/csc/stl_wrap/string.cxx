module;
#include <cstdint>
#include <string>
export module csc.stl_wrap.string;

export namespace csc {

template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
using basic_string = std::basic_string<CharT, Traits, Alloc>;

using string = csc::basic_string<char>;
using wstring = csc::basic_string<wchar_t>;
using u8string = csc::basic_string<char8_t>;
using u16string = csc::basic_string<char16_t>;
using u32string = csc::basic_string<char32_t>;

#define BASIC_STRING csc::basic_string<CharT, Traits, Alloc>

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

int stoi(const csc::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoi(str, pos, base);
}
int stoi(const csc::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoi(str, pos, base);
}
long stol(const csc::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stol(str, pos, base);
}
long stol(const csc::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stol(str, pos, base);
}
long long stoll(const csc::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoll(str, pos, base);
}
long long stoll(const csc::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoll(str, pos, base);
}

unsigned long stoul(const csc::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoul(str, pos, base);
}
unsigned long stoul(const csc::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoul(str, pos, base);
}
unsigned long long stoull(const csc::string& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoull(str, pos, base);
}
unsigned long long stoull(const csc::wstring& str, std::size_t* pos = nullptr, int base = 10) {
  return std::stoull(str, pos, base);
}

float stof(const csc::string& str, std::size_t* pos = nullptr) {
  return std::stof(str, pos);
}
float stof(const csc::wstring& str, std::size_t* pos = nullptr) {
  return std::stof(str, pos);
}
double stod(const csc::string& str, std::size_t* pos = nullptr) {
  return std::stod(str, pos);
}
double stod(const csc::wstring& str, std::size_t* pos = nullptr) {
  return std::stod(str, pos);
}
long double stold(const csc::string& str, std::size_t* pos = nullptr) {
  return std::stold(str, pos);
}
long double stold(const csc::wstring& str, std::size_t* pos = nullptr) {
  return std::stold(str, pos);
}

csc::string to_string(int value) {
  return std::to_string(value);
}
csc::string to_string(long value) {
  return std::to_string(value);
}
csc::string to_string(long long value) {
  return std::to_string(value);
}
csc::string to_string(unsigned value) {
  return std::to_string(value);
}
csc::string to_string(unsigned long value) {
  return std::to_string(value);
}
csc::string to_string(unsigned long long value) {
  return std::to_string(value);
}
csc::string to_string(float value) {
  return std::to_string(value);
}
csc::string to_string(double value) {
  return std::to_string(value);
}
csc::string to_string(long double value) {
  return std::to_string(value);
}

csc::wstring to_wstring(int value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(long value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(long long value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(unsigned value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(unsigned long value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(unsigned long long value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(float value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(double value) {
  return std::to_wstring(value);
}
csc::wstring to_wstring(long double value) {
  return std::to_wstring(value);
}

// csc::pmr
namespace pmr {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string = csc::basic_string<CharT, Traits, std::pmr::polymorphic_allocator<CharT>>;

using string = csc::pmr::basic_string<char>;
using wstring = csc::pmr::basic_string<wchar_t>;
using u8string = csc::pmr::basic_string<char8_t>;
using u16string = csc::pmr::basic_string<char16_t>;
using u32string = csc::pmr::basic_string<char32_t>;

} // namespace pmr

// csc::string_literals
inline namespace string_literals {
constexpr inline csc::string operator""_s(const char* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline csc::wstring operator""_s(const wchar_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline csc::u8string operator""_s(const char8_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline csc::u16string operator""_s(const char16_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}
constexpr inline csc::u32string operator""_s(const char32_t* str, std::size_t len) {
  return std::string_literals::operator""s(str, len);
}

} // namespace string_literals

} // namespace csc