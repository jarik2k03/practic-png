module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <utility>
export module csc.wnd.window_handler;

import stl.stdexcept;
import stl.string_view;
import stl.vector;

namespace csc {
namespace wnd {

export using glfw_window = GLFWwindow;

export class window_handler {
 private:
  GLFWwindow* m_window = nullptr;

 public:
  window_handler() = default;
  ~window_handler() noexcept;
  window_handler(window_handler&& move) noexcept;
  window_handler& operator=(window_handler&& move) noexcept;
  window_handler(uint32_t width, uint32_t height, std::string_view name);
  VkSurfaceKHR create_surface(VkInstance instance) const;
  bool should_close() const;
  void set_size_limits(VkExtent2D min);
  void set_size_limits(VkExtent2D min, VkExtent2D max);
  void set_user_pointer(void* user_data);
  void set_framebuffer_size_callback(GLFWframebuffersizefun callback);

  void* get_user_pointer() const;
  VkExtent2D get_framebuffer_size() const;
  std::vector<const char*> get_required_instance_extensions() const;
};

std::vector<const char*> window_handler::get_required_instance_extensions() const {
  uint32_t req_extension_count;
  const char** req_extensions = ::glfwGetRequiredInstanceExtensions(&req_extension_count);
  std::vector<const char*> extensions;
  for (auto idx = 0u; idx < req_extension_count; idx++) {
    const char* const ext = req_extensions[idx];
    extensions.emplace_back(ext);
  }
  return extensions;
}

window_handler::~window_handler() noexcept {
  if (m_window != nullptr)
    ::glfwDestroyWindow(m_window), m_window = nullptr;
}

window_handler::window_handler(window_handler&& move) noexcept : m_window(std::exchange(move.m_window, nullptr)) {
}

window_handler& window_handler::operator=(window_handler&& move) noexcept {
  if (&move == this)
    return *this;
  m_window = std::exchange(move.m_window, nullptr);
  return *this;
}

window_handler::window_handler(uint32_t width, uint32_t height, std::string_view name) {
  ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  m_window = ::glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
}

bool window_handler::should_close() const {
  [[unlikely]] if (m_window == nullptr)
    throw std::runtime_error("Window handler: невозможно узнать о закрытии окна, пока оно не создано.");
  return ::glfwWindowShouldClose(m_window);
}

VkSurfaceKHR window_handler::create_surface(VkInstance instance) const {
  [[unlikely]] if (m_window == nullptr)
    throw std::runtime_error("Window handler: невозможно создать surface, пока не создано окно.");
  VkSurfaceKHR new_surface;
  const auto err = ::glfwCreateWindowSurface(instance, m_window, nullptr, &new_surface);
  [[unlikely]] if (err != VK_SUCCESS)
    throw std::runtime_error("Window handler: не удалось создать surface!");
  return new_surface;
}

void window_handler::set_size_limits(VkExtent2D min) {
  ::glfwSetWindowSizeLimits(m_window, min.width, min.height, GLFW_DONT_CARE, GLFW_DONT_CARE);
}
void window_handler::set_size_limits(VkExtent2D min, VkExtent2D max) {
  ::glfwSetWindowSizeLimits(m_window, min.width, min.height, max.width, max.height);
}
void window_handler::set_user_pointer(void* user_data) {
  ::glfwSetWindowUserPointer(m_window, user_data);
}
void* window_handler::get_user_pointer() const {
  return ::glfwGetWindowUserPointer(m_window);
}
VkExtent2D window_handler::get_framebuffer_size() const {
  int w, h;
  ::glfwGetFramebufferSize(m_window, &w, &h);
  return VkExtent2D{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
}

void window_handler::set_framebuffer_size_callback(GLFWframebuffersizefun callback) {
  ::glfwSetFramebufferSizeCallback(m_window, callback);
}

} // namespace wnd
} // namespace csc
