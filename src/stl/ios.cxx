module;
#include <cstdint>
#include <ios>
export module stl.ios;

export namespace std {

using std::basic_ios;
using std::fpos;
using std::ios_base;

using std::ios;
using std::wios;

using std::boolalpha;
using std::noboolalpha;

using std::noshowbase;
using std::showbase;

using std::noshowpoint;
using std::showpoint;

using std::noshowpos;
using std::showpos;

using std::noskipws;
using std::skipws;

using std::nouppercase;
using std::uppercase;

using std::nounitbuf;
using std::unitbuf;
// adjustfield
using std::internal;
using std::left;
using std::right;
// basefield
using std::dec;
using std::hex;
using std::oct;
// floatfield
using std::defaultfloat;
using std::fixed;
using std::hexfloat;
using std::scientific;

// error reporting
using std::error_category;
using std::error_code;
using std::error_condition;
using std::io_errc;

using std::iostream_category;
using std::is_error_code_enum;
using std::make_error_code;
using std::make_error_condition;

} // namespace std
