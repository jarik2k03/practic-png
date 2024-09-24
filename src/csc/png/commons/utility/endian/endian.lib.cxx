module;
#include <cstdint>
#include <bit>
export module csc.png.commons.utility.endian;
export import :attributes;

export namespace csc {
namespace png {

consteval bool is_valid_endian() noexcept;

template <png::number T>
T swap_endian(T num) noexcept;

} // namespace png
} // namespace csc

namespace csc {
namespace png {

consteval bool is_valid_endian() noexcept {
  using enum std::endian;
  return native == big || native == little;
}

template <png::number T>
T swap_endian(T num) noexcept {
  union {
    T t_value;
    uint8_t bytes[sizeof(T)];
  } source, dest;

  source.t_value = num; // заполняем source
  for (auto k = 0u; k < sizeof(T); k++) // из source в desc в обр. порядке
    dest.bytes[k] = source.bytes[sizeof(T) - k - 1];

  return dest.t_value;
}
} // namespace png
} // namespace csc
