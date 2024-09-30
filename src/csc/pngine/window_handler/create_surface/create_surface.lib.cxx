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
 private:
  const vk::Instance& m_instance;

 public:
  f_create_surface(const vk::Instance& instance) : m_instance(instance) {
  }
#ifdef _WIN32
  vk::SurfaceKHR operator()(const pngine::win32_handler& win32) const {
    return win32.create_surface(m_instance);
  }
#elif __linux
  vk::SurfaceKHR operator()(const pngine::xcb_handler& xcb) const {
    return xcb.create_surface(m_instance);
  }
#endif
};

} // namespace pngine
} // namespace csc
