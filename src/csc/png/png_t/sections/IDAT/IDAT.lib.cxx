module;
#include <cstdint>
export module csc.png.png_t.sections.IDAT;

export namespace csc {

struct IDAT {
  uint32_t crc_adler{0u};
};

} // namespace csc
