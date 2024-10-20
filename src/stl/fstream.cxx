module;
#include <cstdint>
#include <fstream>
export module stl.fstream;

export namespace std {

using std::basic_fstream;
using std::fstream;
using std::wfstream;

using std::basic_ifstream;
using std::ifstream;
using std::wifstream;

using std::basic_ofstream;
using std::ofstream;
using std::wofstream;

using std::char_traits;
using std::operator|;
} // namespace std
