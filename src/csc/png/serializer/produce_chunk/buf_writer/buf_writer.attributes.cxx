module;
#include <bit>
#include <cstdint>
export module csc.png.serializer.produce_chunk.buf_writer:attributes;

import csc.png.commons.utility.endian;

export namespace csc {
namespace png {

template <png::number T>
T from_system_endian_to_be(T num) {
  using enum std::endian;
  if constexpr (native == little)
    return png::swap_endian(num);
  else
    return num;
}

} // namespace png
} // namespace csc
