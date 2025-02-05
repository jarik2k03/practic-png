module;
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cmath>
export module csc.wnd;

import csc.pngine;
import csc.png.picture;

export import csc.wnd.window_handler;
export import csc.wnd.glfw_handler;
export import csc.wnd.params_parser;
export import csc.wnd.controller;

import stl.iostream;
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

void switch_from_normal_to_insert(
    wnd::window_handler& window,
    wnd::controller*& to_fill,
    bool first_btn,
    bool second_btn,
    bool third_btn,
    bool at_least_one,
    bool need_action) {
  if (at_least_one && need_action) {
    if (first_btn)
      to_fill->current_pressed = wnd::e_toolbox_buttons::clip_image;
    else if (second_btn)
      to_fill->current_pressed = wnd::e_toolbox_buttons::scale_image;
    else if (third_btn)
      to_fill->current_pressed = wnd::e_toolbox_buttons::rotate_image;
    to_fill->current_state = wnd::e_program_state::insert;
    window.set_window_title("Введите параметры или нажмите ПКМ по галочке, чтобы отменить.");
  }
}

void switch_from_insert_to_normal(
    wnd::window_handler& window,
    wnd::controller*& to_fill,
    pngine::pngine_core*& render,
    png::picture*& image_data,
    bool first_btn,
    bool work_action,
    bool abort_action) {
  if (first_btn && work_action) {
    wnd::params_parser parser(to_fill->input_data);
    pngine::image_manipulator_bundle result;
    if (to_fill->current_pressed == wnd::e_toolbox_buttons::clip_image) {
      const auto [a, b, c, d] = parser.parse_clip_params();
      result = render->get_toolbox().clip_image({a, b}, {c, d});
    } else if (to_fill->current_pressed == wnd::e_toolbox_buttons::scale_image) {
      const auto [a, b] = parser.parse_scale_params();
      result = render->get_toolbox().scale_image(a, b);
    } else if (to_fill->current_pressed == wnd::e_toolbox_buttons::rotate_image) {
      const auto [a] = parser.parse_rotate_params();
      result = render->get_toolbox().rotate_image(a);
    }
    image_data->header().width = result.image_size.width, image_data->header().height = result.image_size.height;
    render->change_drawing(result.staged, image_data->header());
  }
  if (first_btn &&
      (work_action || abort_action)) { // abort-нажатие только переключит состояние, а work переключит после работы
    to_fill->input_data.clear(); // очищаем ввод параметров
    to_fill->current_state = wnd::e_program_state::normal;
    window.set_window_title("PNG-обозреватель");
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
  auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] =
      *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  if ((codepoint >= '0' && codepoint <= '9') || codepoint == '-' || codepoint == '.' || codepoint == ' ' ||
      codepoint == '+') // ограничиваем
    controller_obj->input_data.push_back(static_cast<char>(codepoint));

  if (!controller_obj->input_data.empty())
    window_obj.set_window_title(std::string("Ввод: " + controller_obj->input_data).c_str());
  else
    window_obj.set_window_title("Введите параметры или нажмите ПКМ по галочке, чтобы отменить.");
}
export void choosing_tool_by_keyboard_event(
    wnd::glfw_window* current,
    int key,
    int /*scancode*/,
    int action,
    int /*mods*/) noexcept {
  auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] =
      *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());

  const bool is_press_action = (action == GLFW_PRESS);
  const bool is_key_on_clip_btn = (key == GLFW_KEY_C);
  const bool is_key_on_scale_btn = (key == GLFW_KEY_S);
  const bool is_key_on_rotate_btn = (key == GLFW_KEY_R);
  const bool is_one_of_there_btns_clicked = is_key_on_clip_btn | is_key_on_scale_btn | is_key_on_rotate_btn;
  wnd::switch_from_normal_to_insert(
      window_obj,
      controller_obj,
      is_key_on_clip_btn,
      is_key_on_scale_btn,
      is_key_on_rotate_btn,
      is_one_of_there_btns_clicked,
      is_press_action);
}

export void applying_tool_by_keyboard_event(
    wnd::glfw_window* current,
    int key,
    int /*scancode*/,
    int action,
    int /*mods*/) noexcept {
  auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] =
      *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());

  if (key == GLFW_KEY_BACKSPACE && !controller_obj->input_data.empty() &&
      (action == GLFW_REPEAT || action == GLFW_PRESS)) {
    controller_obj->input_data.pop_back(); // также доступно удаление последнего символа из ввода
    if (!controller_obj->input_data.empty())
      window_obj.set_window_title(std::string("Ввод: " + controller_obj->input_data).c_str());
    else
      window_obj.set_window_title("Введите параметры или нажмите ПКМ по галочке, чтобы отменить.");
  }
  const bool is_accept_action = (key == GLFW_KEY_ENTER && action == GLFW_PRESS);
  const bool is_decline_action = (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS);
  const bool is_key_on_apply_btn = (key == GLFW_KEY_ENTER || key == GLFW_KEY_ESCAPE);
  wnd::switch_from_insert_to_normal(
      window_obj, controller_obj, pngine_obj, picture_obj, is_key_on_apply_btn, is_accept_action, is_decline_action);
}

export void choosing_tool_by_mouse_event(wnd::glfw_window* current, int button, int action, int /*mods*/) noexcept {
  auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] =
      *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  const auto precised_cursor_pos = window_obj.get_cursor_pos();
  const auto floored_cursor_pos = glm::ivec2(
      static_cast<int32_t>(std::floor(precised_cursor_pos.x)), static_cast<int32_t>(std::floor(precised_cursor_pos.y)));

  const bool is_left_press_action = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS);
  const bool is_mouse_on_clip_btn =
      is_cursor_in_range(floored_cursor_pos, controller_obj->clip_button_begin, controller_obj->clip_button_end);
  const bool is_mouse_on_scale_btn =
      is_cursor_in_range(floored_cursor_pos, controller_obj->scale_button_begin, controller_obj->scale_button_end);
  const bool is_mouse_on_rotate_btn =
      is_cursor_in_range(floored_cursor_pos, controller_obj->rotate_button_begin, controller_obj->rotate_button_end);
  const bool is_one_of_there_btns_clicked = is_mouse_on_clip_btn | is_mouse_on_scale_btn | is_mouse_on_rotate_btn;
  wnd::switch_from_normal_to_insert(
      window_obj,
      controller_obj,
      is_mouse_on_clip_btn,
      is_mouse_on_scale_btn,
      is_mouse_on_rotate_btn,
      is_one_of_there_btns_clicked,
      is_left_press_action);
}

export void applying_tool_by_mouse_event(wnd::glfw_window* current, int button, int action, int /*mods*/) noexcept {
  auto& window_obj = *reinterpret_cast<wnd::window_handler*>(&current); // пока так можно делать
  auto& [pngine_obj, picture_obj, controller_obj] =
      *reinterpret_cast<wnd::pngine_picture_input_package*>(window_obj.get_user_pointer());
  const auto precised_cursor_pos = window_obj.get_cursor_pos();
  const auto floored_cursor_pos = glm::ivec2(
      static_cast<int32_t>(std::floor(precised_cursor_pos.x)), static_cast<int32_t>(std::floor(precised_cursor_pos.y)));

  const bool is_accept_action = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS);
  const bool is_decline_action = (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS);
  const bool is_mouse_on_apply_btn =
      is_cursor_in_range(floored_cursor_pos, controller_obj->apply_button_begin, controller_obj->apply_button_end);
  wnd::switch_from_insert_to_normal(
      window_obj, controller_obj, pngine_obj, picture_obj, is_mouse_on_apply_btn, is_accept_action, is_decline_action);
}

} // namespace wnd
} // namespace csc
