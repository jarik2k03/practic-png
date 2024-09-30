module;
#include <cstdint>
#include <cstdlib>
export module csc.pngine:attributes;

export import csc.pngine.commons.utility.version;
export import csc.pngine.window_handler;
export import stl.optional;

export namespace csc {
namespace pngine {

using ov_window_handler = std::optional<v_window_handler>;
// engine attributes
consteval const char* bring_engine_name() noexcept {
  return "PNGine";
}
consteval pngine::version bring_engine_version() noexcept {
  return pngine::bring_version(1, 0, 0);
}
pngine::v_window_handler init_window_handler(uint32_t width, uint32_t height) noexcept {
#ifdef __linux
  return pngine::v_window_handler(pngine::xcb_handler(width, height));
#elif _WIN32
  return pngine::v_window_handler(pngine::win32_handler(width, height));
#endif
}

} // namespace pngine
} // namespace csc
