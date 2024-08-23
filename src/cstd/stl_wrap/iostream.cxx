module;
#include <cstdint>
#include <iostream>
export module cstd.stl_wrap.iostream;

export import cstd.stl_wrap.istream;
export import cstd.stl_wrap.ostream;

export namespace cstd {

auto& cout = std::cout;
cstd::ostream& cerr = std::cerr;
cstd::ostream& clog = std::clog;
cstd::istream& cin = std::cin;

cstd::wostream& wcout = std::wcout;
cstd::wostream& wcerr = std::wcerr;
cstd::wostream& wclog = std::wclog;
cstd::wistream& wcin = std::wcin;

} // namespace cstd
