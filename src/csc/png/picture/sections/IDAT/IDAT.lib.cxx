module;
#include <cstdint>
export module csc.png.picture.sections.IDAT;

export namespace csc {

struct IDAT {
  uint32_t crc_adler{0u};
};

} // namespace csc
