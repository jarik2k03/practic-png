module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device;

import stl.optional;
import stl.string_view;

import csc.pngine.commons.utility.graphics_pipeline;
import csc.pngine.instance.device.shader_module;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class device : private device_impl {
 public:
  explicit device() : device_impl() {
  }
  explicit device(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) : device_impl(device, surface) {
  }
  device(device&& move) noexcept : device_impl(std::move(move)) {
  }
  device& operator=(device&& move) noexcept {
    return static_cast<device&>(device_impl::operator=(std::move(move)));
  }
  ~device() noexcept = default;
  // геттеры
  const auto& get_shader_module(std::string_view name) const {
    return this->do_get_shader_module(name);
  }
  const auto& get_swapchain() const {
    return this->do_get_swapchain();
  }
  const auto& get_render_pass(std::string_view name) const {
    return this->do_get_render_pass(name);
  }
  const auto& get_framebuffers() const {
    return this->do_get_framebuffers();
  }
  // сеттеры-креаторы
  void create_swapchainKHR() {
    this->do_create_swapchainKHR();
  }
  void create_image_views() {
    this->do_create_image_views();
  }
  void create_shader_module(std::string_view name, std::string_view compiled_filepath) {
    this->do_create_shader_module(name, compiled_filepath);
  }
  void create_render_pass(std::string_view pass_name) {
    this->do_create_render_pass(pass_name);
  }
  template <pngine::c_graphics_pipeline_config Config>
  auto& create_pipeline(std::string_view search_layout_name, std::string_view search_pass_name, Config&& config) {
    return this->do_create_pipeline(search_layout_name, search_pass_name, std::forward<Config>(config));
  }
  void create_pipeline_layout(std::string_view layout_name) {
    this->do_create_pipeline_layout(layout_name);
  }
  void create_framebuffers(std::string_view pass_name) {
    this->do_create_framebuffers(pass_name);
  }
  auto& create_command_pool() {
    return this->do_create_command_pool();
  }
  void clear() noexcept {
    this->do_clear();
  }
};

} // namespace pngine
} // namespace csc
