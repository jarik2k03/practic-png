module;
#include <cstdint>
export module csc.png.png_t.sections.PLTE;

export import csc.stl_wrap.vector;

export import :attributes;
export namespace csc {

struct PLTE {
  csc::vector<rgb8> full_palette;
  uint32_t crc_adler;
};

} // namespace csc
