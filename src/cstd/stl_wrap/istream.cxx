module;
#include <cstdint>
#include <istream>
export module cstd.stl_wrap.istream;

export namespace cstd {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_istream = std::basic_istream<CharT, Traits>;
using istream = cstd::basic_istream<char>;
using wistream = cstd::basic_istream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_iostream = std::basic_iostream<CharT, Traits>;
using iostream = cstd::basic_iostream<char>;
using wiostream = cstd::basic_iostream<wchar_t>;

template <class CharT, class Traits>
cstd::basic_istream<CharT, Traits>& ws(cstd::basic_istream<CharT, Traits>& is) {
  return std::ws(is);
}

// characters read
template <class CharT, class Traits>
cstd::basic_istream<CharT, Traits>& operator>>(cstd::basic_istream<CharT, Traits>& st, CharT& ch) {
  return std::operator>>(st, ch);
}
template <class Traits>
cstd::basic_istream<char, Traits>& operator>>(cstd::basic_istream<char, Traits>& st, signed char& ch) {
  return std::operator>>(st, ch);
}
template <class Traits>
cstd::basic_istream<char, Traits>& operator>>(cstd::basic_istream<char, Traits>& st, unsigned char& ch) {
  return std::operator>>(st, ch);
}
// c-style strings read
template <class CharT, class Traits, std::size_t N>
cstd::basic_istream<CharT, Traits>& operator>>(cstd::basic_istream<CharT, Traits>& st, CharT (&)[N]) {
  return std::operator>>(st, N);
}
template <class Traits, std::size_t N>
cstd::basic_istream<char, Traits>& operator>>(cstd::basic_istream<char, Traits>& st, signed char (&)[N]) {
  return std::operator>>(st, N);
}
template <class Traits, std::size_t N>
cstd::basic_istream<char, Traits>& operator>>(cstd::basic_istream<char, Traits>& st, unsigned char (&)[N]) {
  return std::operator>>(st, N);
}
// with appropriate operator
template <class Istream, class T>
Istream&& operator>>(Istream&& st, T&& value) {
  return std::operator>>(st, value);
}

} // namespace cstd
