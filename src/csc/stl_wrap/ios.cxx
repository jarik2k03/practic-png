module;
#include <cstdint>
#include <ios>
export module csc.stl_wrap.ios;

export namespace csc {
template <class StateT>
using fpos = std::fpos<StateT>;

using ios_base = std::ios_base;

template <class CharT, class Traits = std::char_traits<CharT>>
using basic_ios = std::basic_ios<CharT, Traits>;

using ios = csc::basic_ios<char>;
using wios = csc::basic_ios<wchar_t>;

// manipulators
csc::ios_base& boolalpha(csc::ios_base& str) {
  return std::boolalpha(str);
}
csc::ios_base& noboolalpha(csc::ios_base& str) {
  return std::noboolalpha(str);
}

csc::ios_base& showbase(csc::ios_base& str) {
  return std::showbase(str);
}
csc::ios_base& noshowbase(csc::ios_base& str) {
  return std::noshowbase(str);
}

csc::ios_base& showpoint(csc::ios_base& str) {
  return std::showpoint(str);
}
csc::ios_base& noshowpoint(csc::ios_base& str) {
  return std::noshowpoint(str);
}

csc::ios_base& showpos(csc::ios_base& str) {
  return std::showpos(str);
}
csc::ios_base& noshowpos(csc::ios_base& str) {
  return std::noshowpos(str);
}

csc::ios_base& skipws(csc::ios_base& str) {
  return std::skipws(str);
}
csc::ios_base& noskipws(csc::ios_base& str) {
  return std::noskipws(str);
}

csc::ios_base& uppercase(csc::ios_base& str) {
  return std::uppercase(str);
}
csc::ios_base& nouppercase(csc::ios_base& str) {
  return std::nouppercase(str);
}

csc::ios_base& unitbuf(csc::ios_base& str) {
  return std::unitbuf(str);
}
csc::ios_base& nounitbuf(csc::ios_base& str) {
  return std::nounitbuf(str);
}

// adjustfield
csc::ios_base& internal(csc::ios_base& str) {
  return std::internal(str);
}
csc::ios_base& left(csc::ios_base& str) {
  return std::left(str);
}
csc::ios_base& right(csc::ios_base& str) {
  return std::right(str);
}

// basefield
csc::ios_base& dec(csc::ios_base& str) {
  return std::dec(str);
}
csc::ios_base& hex(csc::ios_base& str) {
  return std::hex(str);
}
csc::ios_base& oct(csc::ios_base& str) {
  return std::oct(str);
}

// floatfield
csc::ios_base& fixed(csc::ios_base& str) {
  return std::fixed(str);
}
csc::ios_base& scientific(csc::ios_base& str) {
  return std::scientific(str);
}
csc::ios_base& hexfloat(csc::ios_base& str) {
  return std::hexfloat(str);
}
csc::ios_base& defaultfloat(csc::ios_base& str) {
  return std::defaultfloat(str);
}

// error reporting
using io_errc = std::io_errc;

using error_code = std::error_code;
using error_condition = std::error_condition;
using error_category = std::error_category;

template <typename ErrEnum>
using is_error_code_enum = std::is_error_code_enum<ErrEnum>;

csc::error_code make_error_code(csc::io_errc e) noexcept {
  return std::make_error_code(e);
}
csc::error_condition make_error_condition(csc::io_errc e) noexcept {
  return std::make_error_condition(e);
}
const csc::error_category& iostream_category() noexcept {
  return std::iostream_category();
}

} // namespace csc
