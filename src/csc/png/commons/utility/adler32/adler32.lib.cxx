module;

#include <zlib.h>
#include <cstdint>
export module csc.png.commons.utility.adler32;
import csc.png.commons.buffer_view;

import stl.array;

export namespace csc {

uint32_t adler32_for_chunk(const std::array<char, 4>& name, csc::u8buffer_view data) noexcept {
  uint32_t adler = ::adler32(0ul, reinterpret_cast<const uint8_t*>(name.cbegin()), name.size());
  if (data.size() != 0u)
    adler = ::adler32(adler, data.data(), data.size());
  return adler;
};

} // namespace csc
