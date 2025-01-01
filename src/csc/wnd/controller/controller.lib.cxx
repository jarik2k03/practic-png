module;
#include <utility>
#include <cstdint>
export module csc.wnd.controller;

import stl.string;
import stl.string_view;

import glm_hpp;

namespace csc {
namespace wnd {

export enum class e_program_state : bool {
  normal = 0,
  insert = 1
};

export enum class e_toolbox_buttons : uint8_t {
  clip_image = 0,
  scale_image = 1,
  rotate_image = 2
};

export struct controller {
  static constexpr auto button_size = glm::ivec2(64u, 64u);
  static constexpr auto clip_button_begin = glm::ivec2(0, 0);
  static constexpr auto clip_button_end = clip_button_begin + button_size;
  static constexpr auto scale_button_begin = glm::ivec2(0, 64);
  static constexpr auto scale_button_end = scale_button_begin + button_size;
  static constexpr auto rotate_button_begin = glm::ivec2(0, 128);
  static constexpr auto rotate_button_end = rotate_button_begin + button_size;
  static constexpr auto apply_button_begin = glm::ivec2(0, 192);
  static constexpr auto apply_button_end = apply_button_begin + button_size;

  std::string input_data;
  wnd::e_program_state previous_state = wnd::e_program_state::normal;
  wnd::e_program_state current_state = wnd::e_program_state::normal;
  wnd::e_toolbox_buttons current_pressed;
};


} // namespace wnd
} // namespace csc

