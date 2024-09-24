module;
#include <cstdint>
export module csc.png.picture.sections.IHDR;

export import :attributes;

export namespace csc {
namespace png {
struct IHDR {
  uint32_t width, height;
  uint8_t bit_depth;
  png::e_color_type color_type;
  png::e_compression compression;
  png::e_filter filter;
  png::e_interlace interlace;
};

} // namespace png
} // namespace csc
