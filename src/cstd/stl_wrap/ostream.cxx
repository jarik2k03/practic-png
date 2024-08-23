module;
#include <cstdint>
#include <ostream>
export module cstd.stl_wrap.ostream;

export namespace cstd {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ostream = std::basic_ostream<CharT, Traits>;
using ostream = cstd::basic_ostream<char>;
using wostream = cstd::basic_ostream<wchar_t>;

template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& endl(cstd::basic_ostream<CharT, Traits>& os) {
  return std::endl(os);
}
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& ends(cstd::basic_ostream<CharT, Traits>& os) {
  return std::ends(os);
}
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& flush(cstd::basic_ostream<CharT, Traits>& os) {
  return std::flush(os);
}

template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& emit_on_flush(cstd::basic_ostream<CharT, Traits>& os) {
  return std::emit_on_flush(os);
}
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& noemit_on_flush(cstd::basic_ostream<CharT, Traits>& os) {
  return std::noemit_on_flush(os);
}
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& flush_emit(cstd::basic_ostream<CharT, Traits>& os) {
  return std::flush_emit(os);
}

// character out
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& operator<<(cstd::basic_ostream<CharT, Traits>& os, CharT ch) {
  return std::operator<<(os, ch);
}
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& operator<<(cstd::basic_ostream<CharT, Traits>& os, char ch) {
  return std::operator<<(os, ch);
}
template <class Traits>
cstd::basic_ostream<char, Traits>& operator<<(cstd::basic_ostream<char, Traits>& os, char ch) {
  return std::operator<<(os, ch);
}
template <class Traits>
cstd::basic_ostream<char, Traits>& operator<<(cstd::basic_ostream<char, Traits>& os, signed char ch) {
  return std::operator<<(os, ch);
}
template <class Traits>
cstd::basic_ostream<char, Traits>& operator<<(cstd::basic_ostream<char, Traits>& os, unsigned char ch) {
  return std::operator<<(os, ch);
}
// c-style string out
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& operator<<(cstd::basic_ostream<CharT, Traits>& os, const CharT* s) {
  return std::operator<<(os, s);
}
template <class CharT, class Traits>
cstd::basic_ostream<CharT, Traits>& operator<<(cstd::basic_ostream<CharT, Traits>& os, const char* s) {
  return std::operator<<(os, s);
}
template <class Traits>
cstd::basic_ostream<char, Traits>& operator<<(cstd::basic_ostream<char, Traits>& os, const char* s) {
  return std::operator<<(os, s);
}
template <class Traits>
cstd::basic_ostream<char, Traits>& operator<<(cstd::basic_ostream<char, Traits>& os, const signed char* s) {
  return std::operator<<(os, s);
}
template <class Traits>
cstd::basic_ostream<char, Traits>& operator<<(cstd::basic_ostream<char, Traits>& os, const unsigned char* s) {
  return std::operator<<(os, s);
}
// appropriate insertion operator
template <class Ostream, class T>
Ostream&& operator<<(Ostream&& os, const T& value) {
  return std::operator<<(os, value);
}

} // namespace cstd
