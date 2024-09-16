module;
#include <cstdint>
#include <iostream>
export module stl.stl_wrap.iostream;

export import stl.stl_wrap.istream;
export import stl.stl_wrap.ostream;

export namespace std {

using std::cout;
using std::cerr;
using std::clog;
using std::cin;

using std::wcout;
using std::wcerr;
using std::wclog;
using std::wcin;

} // namespace stl
