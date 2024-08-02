module;
#include <cstdint>
#include <fstream>
export module csc.stl_wrap.fstream;

export namespace csc {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_fstream = std::basic_fstream<CharT, Traits>;
using fstream = csc::basic_fstream<char>;
using wfstream = csc::basic_fstream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ifstream = std::basic_ifstream<CharT, Traits>;
using ifstream = csc::basic_ifstream<char>;
using wifstream = csc::basic_ifstream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ofstream = std::basic_ofstream<CharT, Traits>;
using ofstream = csc::basic_ofstream<char>;
using wofstream = csc::basic_ofstream<wchar_t>;

} // namespace csc
