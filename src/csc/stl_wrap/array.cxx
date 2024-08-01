module;
#include <array>
export module csc.stl_wrap.array;

export namespace csc {

template <typename Tp, std::size_t N>
class basic_array : public std::array<Tp, N> {};

template <typename Tp, std::size_t N>
using array = basic_array<Tp, N>;
} // namespace csc