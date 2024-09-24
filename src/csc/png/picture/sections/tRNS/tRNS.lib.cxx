module;
#include <cstdint>
export module csc.png.picture.sections.tRNS;

export import :attributes;

export namespace csc {
namespace png {

struct tRNS {
  png::v_pixel_view_trns color;
  png::e_pixel_view_trns_id color_type;
};

} // namespace png
} // namespace csc
