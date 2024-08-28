module;
#include <bit>
#include <cstdint>
#include <type_traits>
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

template <csc::number T>
T from_be_to_system_endian(T num) {
  using enum std::endian;
  if (native == little)
    return csc::swap_endian(num);
  else
    return num;
}

} // namespace csc
