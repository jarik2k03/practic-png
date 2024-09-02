module;
#include <bit>
#include <cstdint>
export module csc.png.serializer.produce_chunk.buf_writer:attributes;

export import csc.png.commons.utility;

export namespace csc {

template <csc::number T>
T from_system_endian_to_be(T num) {
  using enum std::endian;
  if constexpr (native == little)
    return csc::swap_endian(num);
  else
    return num;
}

} // namespace csc
