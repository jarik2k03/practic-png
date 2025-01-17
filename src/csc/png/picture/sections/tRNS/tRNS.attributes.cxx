module;
#include <cstdint>
export module csc.png.picture.sections.tRNS:attributes;

export import stl.variant;
export import stl.vector;
export import csc.png.commons.utility.pixel_formats;

export namespace csc {
namespace png {

using v_pixel_view_trns = std::variant<rgb8, rgb16, bw8, bw16, std::vector<plte_index>>;

enum class e_pixel_view_trns_id : uint8_t {
  rgb8 = 0,
  rgb16,
  bw8,
  bw16,
  plte_indices
};

} // namespace png
} // namespace csc
