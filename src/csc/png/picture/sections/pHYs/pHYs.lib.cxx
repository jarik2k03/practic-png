module;
#include <cstdint>
export module csc.png.picture.sections.pHYs;

export import :attributes;
export namespace csc {

struct pHYs {
  uint32_t pixels_per_unit_x, pixels_per_unit_y;
  csc::unit_specifier unit_type;
};

} // namespace csc
