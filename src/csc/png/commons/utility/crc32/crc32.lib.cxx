module;

#include <zlib.h>
#include <cstdint>
export module csc.png.commons.utility.crc32;
import csc.png.commons.buffer_view;

import stl.array;

export namespace csc {
namespace png {

uint32_t crc32_for_chunk(const std::array<char, 4>& name, png::u8buffer_view data) noexcept {
  uint32_t crc = ::crc32(0ul, reinterpret_cast<const uint8_t*>(name.cbegin()), name.size());
  if (data.size() != 0u)
    crc = ::crc32(crc, data.data(), data.size());
  return crc;
};

} // namespace png
} // namespace csc
