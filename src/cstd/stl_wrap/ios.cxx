module;
#include <cstdint>
#include <ios>
export module stl.stl_wrap.ios;

export namespace std {

using std::fpos;
using std::ios_base;
using std::basic_ios;

using std::ios;
using std::wios;

using std::boolalpha;
using std::noboolalpha;

using std::showbase;
using std::noshowbase;

using std::showpoint;
using std::noshowpoint;

using std::showpos;
using std::noshowpos;

using std::skipws;
using std::noskipws;

using std::uppercase;
using std::nouppercase;

using std::unitbuf;
using std::nounitbuf;
// adjustfield
using std::internal;
using std::left;
using std::right;
// basefield
using std::dec;
using std::hex;
using std::oct;
// floatfield
using std::fixed;
using std::scientific;
using std::hexfloat;
using std::defaultfloat;

// error reporting
using std::io_errc;
using std::error_code;
using std::error_condition;
using std::error_category;

using std::is_error_code_enum;
using std::make_error_code;
using std::make_error_condition;
using std::iostream_category;

} // namespace stl
