module;
#include <cstdint>
export module csc.png.png_t.sections:attributes;

export namespace csc {

enum class compression_t : uint8_t {
  deflate = 0
};
enum class interlace_t : uint8_t {
  none = 0,
  adam7 = 1
};
enum class color_type_t : uint8_t {
  bw = 0,
  rgb = 2,
  indexed = 3,
  bwa = 4,
  rgba = 6
};
enum class filter_t : uint8_t {
  adaptive = 0
};

} // namespace csc
