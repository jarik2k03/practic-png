module;
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cmath>
export module csc.wnd;

import csc.pngine;
import csc.png.picture;
export import csc.wnd.window_handler;
export import csc.wnd.glfw_handler;

import stl.iostream;

import glm_hpp;
import vulkan_hpp;

export namespace csc {
namespace wnd {
using pngine_picture_package = std::pair<pngine::pngine_core*, png::picture*>;
}
} // namespace csc

namespace csc {
namespace wnd {

bool is_cursor_in_range(glm::ivec2 cursor_pos, glm::ivec2 begin, glm::ivec2 end) {
  return (cursor_pos.x >= begin.x && cursor_pos.y >= begin.y) && (cursor_pos.x <= end.x && cursor_pos.y <= end.y);
}

export void
resize_framebuffer_event(wnd::glfw_window* current, int framebuffer_width, int framebuffer_height) noexcept {
  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, _] = *reinterpret_cast<wnd::pngine_picture_package*>(window_obj.get_user_pointer());
  const auto size = vk::Extent2D(static_cast<uint32_t>(framebuffer_width), static_cast<uint32_t>(framebuffer_height));
  pngine_obj->recreate_swapchain(size);
}

export void cursor_click_event(wnd::glfw_window* current, int button, int action, int /*mods*/) noexcept {
  constexpr const auto button_size = glm::ivec2(64u, 64u);
  constexpr const auto clip_button_begin = glm::ivec2(0, 0), clip_button_end = clip_button_begin + button_size;
  constexpr const auto scale_button_begin = glm::ivec2(0, 64), scale_button_end = scale_button_begin + button_size;
  constexpr const auto rotate_button_begin = glm::ivec2(0, 128), rotate_button_end = rotate_button_begin + button_size;

  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj] = *reinterpret_cast<wnd::pngine_picture_package*>(window_obj.get_user_pointer());
  const auto precised_cursor_pos = window_obj.get_cursor_pos();
  const auto floored_cursor_pos = glm::ivec2(
      static_cast<int32_t>(std::floor(precised_cursor_pos.x)), static_cast<int32_t>(std::floor(precised_cursor_pos.y)));

  if (is_cursor_in_range(floored_cursor_pos, clip_button_begin, clip_button_end) && button == GLFW_MOUSE_BUTTON_1 &&
      action == GLFW_PRESS) {
    const auto clipped = pngine_obj->get_toolbox().clip_image({0u, 0u}, {210u, 210u});
    picture_obj->header().width = clipped.image_size.width, picture_obj->header().height = clipped.image_size.height;
    pngine_obj->change_drawing(clipped.staged, picture_obj->header());
    std::cout << "Clipped successfully!!!\n";
  } else if (
      is_cursor_in_range(floored_cursor_pos, scale_button_begin, scale_button_end) && button == GLFW_MOUSE_BUTTON_1 &&
      action == GLFW_PRESS) {
    const auto scaled = pngine_obj->get_toolbox().scale_image(1.8f, 2.f);
    picture_obj->header().width = scaled.image_size.width, picture_obj->header().height = scaled.image_size.height;
    pngine_obj->change_drawing(scaled.staged, picture_obj->header());
    std::cout << "Scaled successfully!!!\n";
  } else if (
      is_cursor_in_range(floored_cursor_pos, rotate_button_begin, rotate_button_end) && button == GLFW_MOUSE_BUTTON_1 &&
      action == GLFW_PRESS) {
    const auto rotated = pngine_obj->get_toolbox().rotate_image(glm::pi<float>() / 3);
    picture_obj->header().width = rotated.image_size.width, picture_obj->header().height = rotated.image_size.height;
    pngine_obj->change_drawing(rotated.staged, picture_obj->header());
    std::cout << "Rotated successfully!!!\n";
  }
}

} // namespace wnd
} // namespace csc
