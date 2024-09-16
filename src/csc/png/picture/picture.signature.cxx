module;
#include <cstdint>
export module csc.png.picture:signature;

export import stl.array;

export namespace csc {

struct png_signature {
  std::array<uint8_t, 8> data = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
  int8_t eof_byte() const {
    return data[6];
  }
};

constexpr inline bool operator==(const csc::png_signature& one, const csc::png_signature& two) {
  return one.data == two.data;
}
constexpr inline bool operator!=(const csc::png_signature& one, const csc::png_signature& two) {
  return !(one == two);
}
} // namespace csc
