module;
#include <cstdint>
#include <istream>
export module csc.stl_wrap.istream;

export namespace csc {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_istream = std::basic_istream<CharT, Traits>;
using istream = csc::basic_istream<char>;
using wistream = csc::basic_istream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_iostream = std::basic_iostream<CharT, Traits>;
using iostream = csc::basic_iostream<char>;
using wiostream = csc::basic_iostream<wchar_t>;

template <class CharT, class Traits>
csc::basic_istream<CharT, Traits>& ws(csc::basic_istream<CharT, Traits>& is) {
  return std::ws(is);
}

// characters read
template <class CharT, class Traits>
csc::basic_istream<CharT, Traits>& operator>>(csc::basic_istream<CharT, Traits>& st, CharT& ch) {
  return std::operator>>(st, ch);
}
template <class Traits>
csc::basic_istream<char, Traits>& operator>>(csc::basic_istream<char, Traits>& st, signed char& ch) {
  return std::operator>>(st, ch);
}
template <class Traits>
csc::basic_istream<char, Traits>& operator>>(csc::basic_istream<char, Traits>& st, unsigned char& ch) {
  return std::operator>>(st, ch);
}
// c-style strings read
template <class CharT, class Traits, std::size_t N>
csc::basic_istream<CharT, Traits>& operator>>(csc::basic_istream<CharT, Traits>& st, CharT (&s)[N]) {
  return std::operator>>(st, N);
}
template <class Traits, std::size_t N>
csc::basic_istream<char, Traits>& operator>>(csc::basic_istream<char, Traits>& st, signed char (&s)[N]) {
  return std::operator>>(st, N);
}
template <class Traits, std::size_t N>
csc::basic_istream<char, Traits>& operator>>(csc::basic_istream<char, Traits>& st, unsigned char (&s)[N]) {
  return std::operator>>(st, N);
}
// with appropriate operator
template <class Istream, class T>
Istream&& operator>>(Istream&& st, T&& value) {
  return std::operator>>(st, value);
}

} // namespace csc
