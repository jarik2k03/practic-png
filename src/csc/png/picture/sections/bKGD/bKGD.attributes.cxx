module;
#include <cstdint>
export module csc.png.picture.sections.bKGD:attributes;

export import cstd.stl_wrap.variant;

export namespace csc {

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

using v_pixel_view = cstd::variant<rgb8, rgb16, bw8, bw16, plte_index>;

enum class e_pixel_view_id : uint8_t {
  rgb8 = 0, rgb16, bw8, bw16, plte_index
};


}
