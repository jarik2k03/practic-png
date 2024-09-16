module;
#include <cstdint>
#include <ostream>
export module stl.ostream;

export namespace std {
using std::basic_ostream;
using std::ostream;
using std::wostream;
using std::char_traits;
using std::endl;
using std::ends;
using std::flush;
using std::emit_on_flush;
using std::noemit_on_flush;
using std::flush_emit;

using std::operator<<;

} // namespace stl
