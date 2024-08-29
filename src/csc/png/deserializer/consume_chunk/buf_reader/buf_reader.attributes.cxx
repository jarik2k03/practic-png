module;
#include <bit>
#include <cstdint>
export module csc.png.deserializer.consume_chunk.buf_reader:attributes;

export import csc.png.commons.utility;

export namespace csc {

template <csc::number T>
T from_be_to_system_endian(T num) {
  using enum std::endian;
  if (native == little)
    return csc::swap_endian(num);
  else
    return num;
}

} // namespace csc
