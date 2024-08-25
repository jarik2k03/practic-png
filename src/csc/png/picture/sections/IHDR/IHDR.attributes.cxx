module;
#include <cstdint>
export module csc.png.picture.sections.IHDR:attributes;

export namespace csc {

enum class e_compression : uint8_t {
  deflate = 0
};
enum class e_interlace : uint8_t {
  none = 0,
  adam7 = 1
};
enum class e_color_type : uint8_t {
  bw = 0,
  rgb = 2,
  indexed = 3,
  bwa = 4,
  rgba = 6
};
enum class e_filter : uint8_t {
  adaptive = 0
};

} // namespace csc
