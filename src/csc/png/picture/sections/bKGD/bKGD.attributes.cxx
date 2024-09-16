module;
#include <cstdint>
export module csc.png.picture.sections.bKGD:attributes;

export import stl.stl_wrap.variant;
export import csc.png.commons.utility.pixel_formats;

export namespace csc {

using v_pixel_view = std::variant<rgb8, rgb16, bw8, bw16, plte_index>;

enum class e_pixel_view_id : uint8_t {
  rgb8 = 0,
  rgb16,
  bw8,
  bw16,
  plte_index
};

} // namespace csc
