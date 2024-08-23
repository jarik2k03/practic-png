module;
#include <cstdint>
export module csc.png.png_t:signature;

export import csc.stl_wrap.array;

namespace csc {
export struct png_signature {
  const csc::array<uint8_t, 8> data = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
  bool operator==(const csc::png_signature& other) {
    return data == other.data;
  }
  bool operator!=(const csc::png_signature& other) {
    return !(*this == other);
  }
};

} // namespace csc
