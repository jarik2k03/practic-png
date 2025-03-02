module;
#include <cstdint>
#include <cmath>
export module csc.png.commons.filtering:attributes;

export namespace csc {
namespace png {

enum e_filter_types : uint8_t {
  none = 0u,
  sub = 1u,
  up = 2u,
  average = 3u,
  paeth = 4u
};

constexpr int32_t average_arithmetic(int32_t a, int32_t b) noexcept {
  return (a + b) >> 1u;
}

constexpr uint8_t paeth_predictor(uint8_t a, uint8_t b, uint8_t c) noexcept {
  short pa = std::abs(b - c);
  short pb = std::abs(a - c);
  /* writing it out like this compiles to something faster than introducing a temp variable*/
  const short pc = std::abs(a + b - c - c);
  /* return input value associated with smallest of pa, pb, pc (with certain priority if equal) */
  if (pb < pa) {
    a = b;
    pa = pb;
  }
  return (pc < pa) ? c : a;
}

} // namespace png
} // namespace csc
