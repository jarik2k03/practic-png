export module csc.pngine.window_handler.create_surface;

#ifdef _WIN32
export import csc.pngine.window_handler.win32_handler;
#elif __linux
export import csc.pngine.window_handler.xcb_handler;
#endif
import vulkan_hpp;

export namespace csc {
namespace pngine {

class f_create_surface {
 public:
  constexpr f_create_surface() = default;
#ifdef _WIN32
  vk::SurfaceKHR operator()(const pngine::win32_handler& win32) const {
    return {};
  }
#elif __linux
  vk::SurfaceKHR operator()(const pngine::xcb_handler& xcb) const {
    return {};
  }
#endif
};

} // namespace pngine
} // namespace csc
