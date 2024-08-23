module;
#include <cstdint>
#include <ios>
export module cstd.stl_wrap.ios;

export namespace cstd {

template <class StateT>
using fpos = std::fpos<StateT>;

using ios_base = std::ios_base;

template <class CharT, class Traits = std::char_traits<CharT>>
using basic_ios = std::basic_ios<CharT, Traits>;

using ios = cstd::basic_ios<char>;
using wios = cstd::basic_ios<wchar_t>;

// manipulators
cstd::ios_base& boolalpha(cstd::ios_base& str) {
  return std::boolalpha(str);
}
cstd::ios_base& noboolalpha(cstd::ios_base& str) {
  return std::noboolalpha(str);
}

cstd::ios_base& showbase(cstd::ios_base& str) {
  return std::showbase(str);
}
cstd::ios_base& noshowbase(cstd::ios_base& str) {
  return std::noshowbase(str);
}

cstd::ios_base& showpoint(cstd::ios_base& str) {
  return std::showpoint(str);
}
cstd::ios_base& noshowpoint(cstd::ios_base& str) {
  return std::noshowpoint(str);
}

cstd::ios_base& showpos(cstd::ios_base& str) {
  return std::showpos(str);
}
cstd::ios_base& noshowpos(cstd::ios_base& str) {
  return std::noshowpos(str);
}

cstd::ios_base& skipws(cstd::ios_base& str) {
  return std::skipws(str);
}
cstd::ios_base& noskipws(cstd::ios_base& str) {
  return std::noskipws(str);
}

cstd::ios_base& uppercase(cstd::ios_base& str) {
  return std::uppercase(str);
}
cstd::ios_base& nouppercase(cstd::ios_base& str) {
  return std::nouppercase(str);
}

cstd::ios_base& unitbuf(cstd::ios_base& str) {
  return std::unitbuf(str);
}
cstd::ios_base& nounitbuf(cstd::ios_base& str) {
  return std::nounitbuf(str);
}

// adjustfield
cstd::ios_base& internal(cstd::ios_base& str) {
  return std::internal(str);
}
cstd::ios_base& left(cstd::ios_base& str) {
  return std::left(str);
}
cstd::ios_base& right(cstd::ios_base& str) {
  return std::right(str);
}

// basefield
cstd::ios_base& dec(cstd::ios_base& str) {
  return std::dec(str);
}
cstd::ios_base& hex(cstd::ios_base& str) {
  return std::hex(str);
}
cstd::ios_base& oct(cstd::ios_base& str) {
  return std::oct(str);
}

// floatfield
cstd::ios_base& fixed(cstd::ios_base& str) {
  return std::fixed(str);
}
cstd::ios_base& scientific(cstd::ios_base& str) {
  return std::scientific(str);
}
cstd::ios_base& hexfloat(cstd::ios_base& str) {
  return std::hexfloat(str);
}
cstd::ios_base& defaultfloat(cstd::ios_base& str) {
  return std::defaultfloat(str);
}

// error reporting
using io_errc = std::io_errc;

using error_code = std::error_code;
using error_condition = std::error_condition;
using error_category = std::error_category;

template <typename ErrEnum>
using is_error_code_enum = std::is_error_code_enum<ErrEnum>;

cstd::error_code make_error_code(cstd::io_errc e) noexcept {
  return std::make_error_code(e);
}
cstd::error_condition make_error_condition(cstd::io_errc e) noexcept {
  return std::make_error_condition(e);
}
const cstd::error_category& iostream_category() noexcept {
  return std::iostream_category();
}

} // namespace cstd
