
module;
#include <cstdint>
#include <ctime>
export module csc.png.deserializer.consume_chunk:utility;
import csc.png.picture.sections.IHDR;

export namespace csc {

enum class e_section_code : uint32_t {
  success = 0u,
  error = 1u
};
} // namespace csc

namespace csc {
constexpr uint32_t pixel_size_from_color_type(csc::e_color_type t) {
  using enum csc::e_color_type;
  switch (t) {
    case rgba:
      return 4u;
    case rgb:
      return 3u;
    case bw:
      return 2u;
    case bwa:
      return 3u;
    case indexed:
      return 1u;
    default:
      return 0u;
  }
}

int8_t bring_utc_offset() noexcept {
  std::time_t current_time;
  std::time(&current_time);
  const auto p_localtime = std::localtime(&current_time);
  return static_cast<int8_t>((p_localtime) ? (p_localtime->tm_gmtoff / 60 / 60) : 0); // секунды в часы
}

} // namespace csc
