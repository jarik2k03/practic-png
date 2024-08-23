module;
#include <cstdint>
#include <fstream>
export module cstd.stl_wrap.fstream;

export namespace cstd {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_fstream = std::basic_fstream<CharT, Traits>;
using fstream = cstd::basic_fstream<char>;
using wfstream = cstd::basic_fstream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ifstream = std::basic_ifstream<CharT, Traits>;
using ifstream = cstd::basic_ifstream<char>;
using wifstream = cstd::basic_ifstream<wchar_t>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ofstream = std::basic_ofstream<CharT, Traits>;
using ofstream = cstd::basic_ofstream<char>;
using wofstream = cstd::basic_ofstream<wchar_t>;

} // namespace cstd
