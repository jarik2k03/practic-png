module;
#include <bits/stl_iterator.h>
#include <cstdlib>
module csc.pngine.instance:utility;
import stl.vector;
import stl.string;
import stl.ostream;
export import vulkan_hpp;

namespace csc {
namespace pngine {

consteval bool is_debug_build() noexcept {
#ifndef NDEBUG
  return true;
#else
  return false;
#endif
}

const char* bring_surface_extension() noexcept {
#ifdef _WIN32
  return "VK_KHR_win32_surface";
#else
  const char* const session_ty = ::secure_getenv("XDG_SESSION_TYPE");
  if (session_ty == nullptr) {
    return nullptr;
  }
  const std::string session_type(session_ty);
  if (session_type == "x11") {
    return "VK_KHR_xcb_surface";
  } else if (session_type == "wayland") {
    return "VK_KHR_wayland_surface";
  } else {
    return nullptr;
  }
#endif
  return nullptr;
}

} // namespace pngine
} // namespace csc
