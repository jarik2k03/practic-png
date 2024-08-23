module;
#include <cstdint>
export module csc.png.picture:signature;

export import cstd.stl_wrap.array;

export namespace csc {
using cstd::operator==;

struct png_signature {
  const cstd::array<uint8_t, 8> data = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
};

constexpr inline bool operator==(const csc::png_signature& one, const csc::png_signature& two) {
  return one.data == two.data;
}
constexpr inline bool operator!=(const csc::png_signature& one, const csc::png_signature& two) {
  return !(one == two);
}
} // namespace csc
