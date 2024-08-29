module;

#include <cstdint>
export module csc.png.commons.chunk;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.array;

export namespace csc {
struct chunk {
  cstd::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  cstd::vector<uint8_t> data;
  uint32_t crc_adler = 0u;

  chunk() = default;
  chunk(const chunk& copy) = default;
  chunk& operator=(const chunk& copy) = delete;
};

} // namespace csc
