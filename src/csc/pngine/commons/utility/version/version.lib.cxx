module;
#include <cstdint>
export module csc.pngine.commons.utility.version;

export namespace csc {
namespace pngine {
struct human_version {
  uint16_t major, minor, patch;
};
using version = uint32_t;
constexpr pngine::version bring_version(uint32_t major, uint32_t minor, uint32_t patch) noexcept {
  return pngine::version{major << 22u | minor << 12u | patch};
}
constexpr pngine::human_version bring_human_version(pngine::version packed) noexcept {
  pngine::human_version ver;
  ver.major = (packed >> 22u) & 0b0011'1111'1111;
  ver.minor = (packed >> 12u) & 0b0011'1111'1111;
  ver.patch = packed & 0b1111'1111'1111;
  return ver;
}

} // namespace pngine
} // namespace csc
