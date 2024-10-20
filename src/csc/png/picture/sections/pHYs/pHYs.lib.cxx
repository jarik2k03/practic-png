module;
#include <cstdint>
export module csc.png.picture.sections.pHYs;

export import :attributes;
export namespace csc {
namespace png {

struct pHYs {
  uint32_t pixels_per_unit_x, pixels_per_unit_y;
  png::unit_specifier unit_type;
};

} // namespace png
} // namespace csc
