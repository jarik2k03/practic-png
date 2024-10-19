module;
#include <cstdint>
export module csc.pngine:attributes;

export import csc.pngine.commons.utility.version;

export namespace csc {
namespace pngine {

// engine attributes
consteval const char* bring_engine_name() noexcept {
  return "PNGine";
}
consteval pngine::version bring_engine_version() noexcept {
  return pngine::bring_version(1, 0, 0);
}

} // namespace pngine
} // namespace csc
