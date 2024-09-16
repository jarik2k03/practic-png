module;
#include <iostream>
export module stl.iostream;

export import stl.istream;
export import stl.ostream;

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
