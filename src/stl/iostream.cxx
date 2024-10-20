module;
#include <iostream>
export module stl.iostream;

export import stl.istream;
export import stl.ostream;

export namespace std {

using std::cerr;
using std::cin;
using std::clog;
using std::cout;

using std::wcerr;
using std::wcin;
using std::wclog;
using std::wcout;

} // namespace std
