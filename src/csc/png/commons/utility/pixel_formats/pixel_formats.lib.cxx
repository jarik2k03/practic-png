module;

#include <cstdint>
export module csc.png.commons.utility.pixel_formats;

export namespace csc {
namespace png {

struct rgb8 {
  uint8_t r, g, b;
};
struct rgb16 {
  uint16_t r, g, b;
};
struct bw8 {
  uint8_t bw;
};
struct bw16 {
  uint16_t bw;
};
struct plte_index {
  uint8_t idx;
};

} // namespace png
} // namespace csc
