module;
#include <cstdint>
export module csc.png.picture.sections.tIME:attributes;

export namespace csc {

using second = uint8_t;
using minute = uint8_t;
using hour = uint8_t;

using day = uint8_t;
enum class month : uint8_t {
  january = 1,
  february,
  march,
  april,
  may,
  june,
  july,
  august,
  september,
  october,
  november,
  december
};
using year = uint16_t;
} // namespace csc
