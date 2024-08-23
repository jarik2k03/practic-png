module;
#include <cstdint>
export module csc.png.picture.sections.PLTE;

export import cstd.stl_wrap.vector;

export import :attributes;
export namespace csc {

struct PLTE {
  cstd::vector<rgb8> full_palette;
  uint32_t crc_adler;
};

} // namespace csc
