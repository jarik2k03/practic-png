module;
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cmath>
export module csc.wnd;

import csc.pngine;
import csc.png.picture;

export import csc.wnd.window_handler;
export import csc.wnd.glfw_handler;
export import csc.wnd.controller;

import stl.iostream;
import stl.string;

import glm_hpp;
import vulkan_hpp;

export namespace csc {
namespace wnd {
using pngine_picture_input_package = std::tuple<pngine::pngine_core*, png::picture*, wnd::controller*>;
}
} // namespace csc

namespace csc {
namespace wnd {

bool is_cursor_in_range(glm::ivec2 cursor_pos, glm::ivec2 begin, glm::ivec2 end) {
  return (cursor_pos.x >= begin.x && cursor_pos.y >= begin.y) && (cursor_pos.x <= end.x && cursor_pos.y <= end.y);
}

void switch_from_normal_to_insert(wnd::controller*& to_fill, bool first_btn, bool second_btn, bool third_btn, bool at_least_one, bool need_action) {
  if (at_least_one && need_action) {
    if (first_btn)
      to_fill->current_pressed = wnd::e_toolbox_buttons::clip_image;
    else if (second_btn)
      to_fill->current_pressed = wnd::e_toolbox_buttons::scale_image;
    else if (third_btn)
      to_fill->current_pressed = wnd::e_toolbox_buttons::rotate_image;
    to_fill->current_state = wnd::e_program_state::insert;
  }
}

void switch_from_insert_to_normal(wnd::controller*& to_fill, pngine::pngine_core*& render, png::picture*& image_data, bool first_btn, bool need_action) {
  if (first_btn && need_action) {
    pngine::image_manipulator_bundle result;
    if (to_fill->current_pressed == wnd::e_toolbox_buttons::clip_image) {
      result = render->get_toolbox().clip_image({0u, 0u}, {210u, 210u});
    }
    else if (to_fill->current_pressed == wnd::e_toolbox_buttons::scale_image) {
      result = render->get_toolbox().scale_image(1.8f, 2.f);
    }
    else if (to_fill->current_pressed == wnd::e_toolbox_buttons::rotate_image) {
      result = render->get_toolbox().rotate_image(glm::pi<float>() / 3);
    }
    image_data->header().width = result.image_size.width, image_data->header().height = result.image_size.height;
    render->change_drawing(result.staged, image_data->header());

    to_fill->input_data.clear(); // очищаем ввод параметров
    to_fill->current_state = wnd::e_program_state::normal;
  }
}

export void
resize_framebuffer_event(wnd::glfw_window* current, int framebuffer_width, int framebuffer_height) noexcept {
  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, _, __] = *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  const auto size = vk::Extent2D(static_cast<uint32_t>(framebuffer_width), static_cast<uint32_t>(framebuffer_height));
  pngine_obj->recreate_swapchain(size);
}

export void character_event(wnd::glfw_window* current, uint32_t codepoint) noexcept {
  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] = *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  if (codepoint < 128u) // ограничиваем юникод до 128 значений
    controller_obj->input_data.push_back(static_cast<char>(codepoint));
}

export void choosing_tool_by_mouse_event(wnd::glfw_window* current, int button, int action, int /*mods*/) noexcept {
  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] = *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  const auto precised_cursor_pos = window_obj.get_cursor_pos();
  const auto floored_cursor_pos = glm::ivec2(
      static_cast<int32_t>(std::floor(precised_cursor_pos.x)), static_cast<int32_t>(std::floor(precised_cursor_pos.y)));

  const bool is_left_press_action = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS);
  const bool is_mouse_on_clip_btn = is_cursor_in_range(floored_cursor_pos, controller_obj->clip_button_begin, controller_obj->clip_button_end);
  const bool is_mouse_on_scale_btn = is_cursor_in_range(floored_cursor_pos, controller_obj->scale_button_begin, controller_obj->scale_button_end);
  const bool is_mouse_on_rotate_btn = is_cursor_in_range(floored_cursor_pos, controller_obj->rotate_button_begin, controller_obj->rotate_button_end);
  const bool is_one_of_there_btns_clicked = is_mouse_on_clip_btn | is_mouse_on_scale_btn | is_mouse_on_rotate_btn;
  wnd::switch_from_normal_to_insert(controller_obj, is_mouse_on_clip_btn, is_mouse_on_scale_btn, is_mouse_on_rotate_btn, is_one_of_there_btns_clicked, is_left_press_action);
}

export void applying_tool_by_mouse_event(wnd::glfw_window* current, int button, int action, int /*mods*/) noexcept {
  const auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] = *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  const auto precised_cursor_pos = window_obj.get_cursor_pos();
  const auto floored_cursor_pos = glm::ivec2(
      static_cast<int32_t>(std::floor(precised_cursor_pos.x)), static_cast<int32_t>(std::floor(precised_cursor_pos.y)));

  const bool is_left_press_action = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS);
  const bool is_mouse_on_apply_btn = is_cursor_in_range(floored_cursor_pos, controller_obj->apply_button_begin, controller_obj->apply_button_end);
  wnd::switch_from_insert_to_normal(controller_obj, pngine_obj, picture_obj, is_mouse_on_apply_btn, is_left_press_action);
}

} // namespace wnd
} // namespace csc
