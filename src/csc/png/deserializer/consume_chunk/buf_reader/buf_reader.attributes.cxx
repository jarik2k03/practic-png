module;
#include <bit>
#include <cstdint>
export module csc.png.deserializer.consume_chunk.buf_reader:attributes;

import csc.png.commons.utility.endian;

export namespace csc {

template <csc::number T>
T from_be_to_system_endian(T num) {
  using enum std::endian;
  if constexpr (native == little)
    return csc::swap_endian(num);
  else
    return num;
}

} // namespace csc
