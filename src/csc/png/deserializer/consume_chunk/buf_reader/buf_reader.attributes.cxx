module;
#include <cstdint>
#include <type_traits>
#include <bit>
export module csc.png.deserializer.consume_chunk.buf_reader:attributes;

export namespace csc {

template <typename T>
concept number = std::is_integral_v<T>;

consteval bool is_valid_endian() noexcept {
  using enum std::endian;
  return native == big || native == little;
}

template <csc::number T>
T swap_endian(T num) {
  union {
    T t_value;
    uint8_t bytes[sizeof(T)];
  } source, dest;

  source.t_value = num; // заполняем source
  for (auto k = 0u; k < sizeof(T); k++) // из source в desc в обр. порядке
    dest.bytes[k] = source.bytes[sizeof(T) - k - 1];

  return dest.t_value;
}

}
