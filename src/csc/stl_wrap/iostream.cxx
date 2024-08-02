module;
#include <cstdint>
#include <iostream>
export module csc.stl_wrap.iostream;

export import csc.stl_wrap.istream;
export import csc.stl_wrap.ostream;

export namespace csc {

csc::ostream& cout = std::cout;
csc::ostream& cerr = std::cerr;
csc::ostream& clog = std::clog;
csc::istream& cin = std::cin;

csc::wostream& wcout = std::wcout;
csc::wostream& wcerr = std::wcerr;
csc::wostream& wclog = std::wclog;
csc::wistream& wcin = std::wcin;

} // namespace csc
