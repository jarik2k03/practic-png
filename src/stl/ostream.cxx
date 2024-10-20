module;
#include <cstdint>
#include <ostream>
export module stl.ostream;

export namespace std {
using std::basic_ostream;
using std::char_traits;
using std::emit_on_flush;
using std::endl;
using std::ends;
using std::flush;
using std::flush_emit;
using std::noemit_on_flush;
using std::ostream;
using std::wostream;

using std::operator<<;

} // namespace std
