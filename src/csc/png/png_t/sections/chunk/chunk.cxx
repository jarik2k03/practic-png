module;

#include <cstdint>
export module csc.png.png_t.sections.chunk;

import csc.stl_wrap.vector;
import csc.stl_wrap.array;

export namespace csc {
struct chunk {
  uint32_t contained_length = 0u;
  csc::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  csc::vector<uint8_t> data;
  uint32_t crc_adler = 0u;
};

} // namespace csc
