module;
#include <cstdint>
export module csc.png.png_t.sections.IHDR;

export import :attributes;

export namespace csc {
struct IHDR {
  uint32_t width, height;
  uint8_t bit_depth;
  csc::color_type_t color_type;
  csc::e_compression compression;
  csc::filter_t filter;
  csc::interlace_t interlace;
  uint32_t crc_adler;
};

} // namespace csc
