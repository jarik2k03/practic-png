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
  void create_swapchainKHR() {
    this->do_create_swapchainKHR();
  }
  void create_image_views() {
    this->do_create_image_views();
  }
  void create_shader_module(std::string_view name, std::string_view compiled_filepath) {
    this->do_create_shader_module(name, compiled_filepath);
  }
  const pngine::shader_module& get_shader_module(std::string_view name) const {
    return this->do_get_shader_module(name);
  }
  template <pngine::c_graphics_pipeline_config Config>
  auto& create_pipeline(Config&& config) {
    return this->do_create_pipeline(std::forward<Config>(config));
  }
  void clear() noexcept {
    this->do_clear();
  }
};

} // namespace pngine
} // namespace csc
