module;

#include <cstdint>
export module csc.png.commons.chunk;

import csc.stl_wrap.vector;
import csc.stl_wrap.array;

export namespace csc {
struct chunk {
  csc::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  csc::vector<uint8_t> data;
  uint32_t crc_adler = 0u;

  chunk() = default;
  chunk& operator=(const chunk& copy) = delete;
};

} // namespace csc
