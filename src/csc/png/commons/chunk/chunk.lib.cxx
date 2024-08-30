module;

#include <cstdint>
#include <memory>
export module csc.png.commons.chunk;

import cstd.stl_wrap.array;

export namespace csc {
struct chunk {
  cstd::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  std::unique_ptr<uint8_t[]> data;
  uint32_t crc_adler = 0u;
  
  uint32_t size = 0u;
  chunk() = default;
  chunk(const chunk& copy) = delete;
  chunk& operator=(const chunk& copy) = delete;
};

} // namespace csc
