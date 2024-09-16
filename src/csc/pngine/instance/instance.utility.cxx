module;
#include <cstdint>
#include <cstdlib>
#include <bits/stl_iterator.h>
module csc.pngine.instance:utility;
import stl.stl_wrap.vector;
import stl.stl_wrap.string;
export import vulkan_hpp;

#ifndef NDEBUG
import stl.stl_wrap.iostream;
#endif

namespace csc {
namespace pngine {

const char* bring_surface_extension() noexcept {
#ifndef NDEBUG // для отладки
  using std::operator<<;
#endif

#ifdef _WIN32
#ifndef NDEBUG // для отладки
  std::cout << "[DEBUG]: pngine::bring_surface_extension: " << "VK_KHR_win32_surface" << '\n';
#endif // для отладки
  return "VK_KHR_win32_surface";
#else
  using std::operator==;
  const char* const session_ty = ::secure_getenv("XDG_SESSION_TYPE");
  if (session_ty == nullptr) {
#ifndef NDEBUG // для отладки
    std::cout << "[DEBUG]: pngine::bring_surface_extension: " << "nullptr" << '\n';
#endif // для отладки
    return nullptr;
  }
  const std::string session_type(session_ty);
  if (session_type == "x11") {
#ifndef NDEBUG // для отладки
    std::cout << "[DEBUG]: pngine::bring_surface_extension: " << "VK_KHR_xlib_surface" << '\n';
#endif // для отладки
    return "VK_KHR_xlib_surface";
  } else if (session_type == "wayland") {
#ifndef NDEBUG // для отладки
    std::cout << "[DEBUG]: pngine::bring_surface_extension: " << "VK_KHR_wayland_surface" << '\n';
#endif // для отладки
    return "VK_KHR_wayland_surface";
  } else {
#ifndef NDEBUG // для отладки
    std::cout << "[DEBUG]: pngine::bring_surface_extension: " << "nullptr" << '\n';
#endif // для отладки
    return nullptr;
  }
#endif
#ifndef NDEBUG // для отладки
  std::cout << "[DEBUG]: pngine::bring_surface_extension: " << "nullptr" << '\n';
#endif // для отладки
  return nullptr;
}

} // namespace pngine
} // namespace csc
