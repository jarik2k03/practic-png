module;
#include <cstdint>
export module csc.png.picture.sections.tRNS:attributes;

export import cstd.stl_wrap.variant;
export import cstd.stl_wrap.vector;
export import csc.png.commons.utility.pixel_formats;

export namespace csc {


using v_pixel_view_trns = cstd::variant<rgb8, rgb16, bw8, bw16, cstd::vector<plte_index>>;

enum class e_pixel_view_trns_id : uint8_t {
  rgb8 = 0,
  rgb16,
  bw8,
  bw16,
  plte_indices
};

} // namespace csc

