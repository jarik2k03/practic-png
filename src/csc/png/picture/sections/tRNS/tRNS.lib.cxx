module;
#include <cstdint>
export module csc.png.picture.sections.tRNS;

export import :attributes;

export namespace csc {

struct tRNS {
  csc::v_pixel_view_trns color;
  csc::e_pixel_view_trns_id color_type;
};

} // namespace csc
