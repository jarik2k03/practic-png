module;

export module csc.pngine.window_handler:visitors;

#ifdef _WIN32
export import csc.pngine.window_handler.win32_handler;
#elif __linux
export import csc.pngine.window_handler.xcb_handler;
#endif
import vulkan_hpp;

export namespace csc {
namespace pngine {

template <typename Pred>
class f_size_event;
class f_poll_event;
class f_create_surface;

template <typename Pred>
class f_size_event {
 private:
  Pred m_user_code;

 public:
  f_size_event(Pred uc) : m_user_code(uc) {
  }
#ifdef _WIN32
  auto operator()(pngine::win32_handler& win32) {
    return win32.size_event(m_user_code);
  }
#elif __linux
  auto operator()(pngine::xcb_handler& xcb) {
    return xcb.expose_event(m_user_code);
  }
#endif
};

class f_wait_events {
 public:
  f_wait_events() = default;
#ifdef _WIN32
  auto operator()(pngine::win32_handler& win32) {
    return win32.wait_events();
  }
#elif __linux
  auto operator()(pngine::xcb_handler& xcb) {
    return xcb.wait_events();
  }
#endif
};

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
