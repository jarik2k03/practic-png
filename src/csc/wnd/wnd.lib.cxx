module;
#include <cstdint>
export module csc.wnd;

import csc.pngine;
export import csc.wnd.window_handler;
export import csc.wnd.glfw_handler;

import vulkan_hpp;

namespace csc {
namespace wnd {

export void resize_framebuffer_event(wnd::glfw_window* current, int framebuffer_width, int framebuffer_height) {
  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& pngine_obj = *reinterpret_cast<pngine::pngine_core*>(window_obj.get_user_pointer());
  const auto size = vk::Extent2D(static_cast<uint32_t>(framebuffer_width), static_cast<uint32_t>(framebuffer_height));
  pngine_obj.recreate_swapchain(size);
}

}
}
