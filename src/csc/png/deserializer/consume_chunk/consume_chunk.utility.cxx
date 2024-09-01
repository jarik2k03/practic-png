
module;
#include <cstdint>
#include <ctime>
module csc.png.deserializer.consume_chunk:utility;
import csc.png.picture.sections.IHDR;

namespace csc {

int8_t bring_utc_offset() noexcept {
  std::time_t current_time;
  std::time(&current_time);
  const auto p_localtime = std::localtime(&current_time);
  return static_cast<int8_t>((p_localtime) ? (p_localtime->tm_gmtoff / 60 / 60) : 0); // секунды в часы
}

} // namespace csc
