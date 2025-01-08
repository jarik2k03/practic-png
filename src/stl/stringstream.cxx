module;
#include <cstdint>
#include <sstream>
export module stl.stringstream;

export namespace std {
using std::istringstream;
using std::ostringstream;
using std::stringstream;

using std::wistringstream;
using std::wostringstream;
using std::wstringstream;

using std::stringbuf;
using std::wstringbuf;

using std::swap;
}
