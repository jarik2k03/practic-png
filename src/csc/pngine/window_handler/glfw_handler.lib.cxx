module;
#include <GLFW/glfw3.h>
#include <utility>
export module csc.pngine.glfw_handler;

namespace csc {
namespace pngine {

export class glfw_handler {
 private:
  bool m_is_initialized;

 public:
  glfw_handler();
  ~glfw_handler() noexcept;
  glfw_handler(glfw_handler&& move) noexcept;
  glfw_handler& operator=(glfw_handler&& move) noexcept;
  void poll_events() const;
};
// implementation
glfw_handler::glfw_handler() {
  ::glfwInit();
#ifdef _WIN32
  ::glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
#elif __linux__
  ::glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  if (::glfwPlatformSupported(GLFW_PLATFORM_WAYLAND) == GLFW_TRUE) {
    ::glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
  }
#endif
  m_is_initialized = true;
}
glfw_handler::~glfw_handler() noexcept {
  if (m_is_initialized == true) {
    ::glfwTerminate(), m_is_initialized = false;
  }
}
glfw_handler::glfw_handler(glfw_handler&& move) noexcept
    : m_is_initialized(std::exchange(move.m_is_initialized, false)) {
}
glfw_handler& glfw_handler::operator=(glfw_handler&& move) noexcept {
  if (&move == this)
    return *this;
  m_is_initialized = std::exchange(move.m_is_initialized, false);
  return *this;
}

void glfw_handler::poll_events() const {
  ::glfwPollEvents();
}

} // namespace pngine
} // namespace csc
