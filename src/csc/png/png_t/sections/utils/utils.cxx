module;

#include <climits>
#include <cstdint>
#include <cstring>
#include <vector>
export module csc.png.png_t.sections:utils;

export namespace csc {
template <class T>
T swap_endian(T u) {
  static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

  union {
    T u;
    unsigned char u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (uint32_t k = 0; k < sizeof(T); k++)
    dest.u8[k] = source.u8[sizeof(T) - k - 1];

  return dest.u;
}

template <class T>
void read_var_from_vector(T& t, uint32_t offset, const std::vector<uint8_t>& v) {
  std::memcpy(&t, &v[offset], sizeof(T));
}

template <class T>
void read_var_from_vector_swap(T& t, uint32_t offset, const std::vector<uint8_t>& v) {
  std::memcpy(&t, &v[offset], sizeof(T));
  t = swap_endian(t);
}

} // namespace csc
