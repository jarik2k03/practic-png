module;
#include <cstdint>
export module csc.png.picture.sections.IHDR;

export import :attributes;

export namespace csc {
struct IHDR {
  uint32_t width, height;
  uint8_t bit_depth;
  csc::e_color_type color_type;
  csc::e_compression compression;
  csc::e_filter filter;
  csc::e_interlace interlace;
  uint32_t crc_adler;
};

} // namespace csc
