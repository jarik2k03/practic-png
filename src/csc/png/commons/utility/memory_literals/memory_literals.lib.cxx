module;

#include <cstdint>
export module csc.png.commons.utility.memory_literals;

export namespace csc {
namespace png {
namespace memory_literals {

constexpr inline unsigned long long operator"" _B(unsigned long long koef) {
  return koef;
}
constexpr inline unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024ul;
}
constexpr inline unsigned long long operator"" _MB(unsigned long long koef) {
  return koef * 1024ul * 1024ul;
}

} // namespace memory_literals
} // namespace png
} // namespace csc
