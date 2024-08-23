module;
#include <array>
export module cstd.stl_wrap.array;

export namespace cstd {

template <typename Tp, std::size_t N>
using array = std::array<Tp, N>;

template <typename Tp, std::size_t N>
constexpr inline bool operator==(const cstd::array<Tp, N>& one, const cstd::array<Tp, N>& two) {
  return std::operator==(one, two);
}

template <typename Tp, std::size_t N>
constexpr inline auto operator<=>(const cstd::array<Tp, N>& one, const cstd::array<Tp, N>& two) {
  return std::operator<=>(one, two);
}

} // namespace cstd
