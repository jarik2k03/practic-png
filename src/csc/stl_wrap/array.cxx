module;
#include <array>
export module csc.stl_wrap.array;

export namespace csc {

template <typename Tp, std::size_t N>
using array = std::array<Tp, N>;

template <typename Tp, std::size_t N>
constexpr bool operator==(const csc::array<Tp, N>& one, const csc::array<Tp, N>& two) {
  return std::operator==(one, two);
}

template <typename Tp, std::size_t N>
constexpr auto operator<=>(const csc::array<Tp, N>& one, const csc::array<Tp, N>& two) {
  return std::operator<=>(one, two);
}

} // namespace csc