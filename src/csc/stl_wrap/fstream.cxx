module;
#include <cstdint>
#include <fstream>
export module csc.stl_wrap.fstream;

export namespace csc {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_fstream : public std::basic_fstream<CharT, Traits> {};
using fstream = basic_fstream<char>;
using wfstream = basic_fstream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_ifstream : public std::basic_ifstream<CharT, Traits> {};
using ifstream = basic_ifstream<char>;
using wifstream = basic_ifstream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_ofstream : public std::basic_ofstream<CharT, Traits> {};
using ofstream = basic_ofstream<char>;
using wofstream = basic_ofstream<wchar_t>;

} // namespace csc
