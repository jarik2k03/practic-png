module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.shader_module;

import stl.string_view;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class shader_module : private shader_module_impl {
 public:
  explicit shader_module() : shader_module_impl() {
  }
  explicit shader_module(const vk::Device& device, std::string_view compiled_filename)
      : shader_module_impl(device, compiled_filename) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  vk::PipelineShaderStageCreateInfo create_shader_stage(
      std::string_view entry_point,
      vk::ShaderStageFlagBits selected_stage) const {
    return this->do_create_shader_stage(entry_point, selected_stage);
  }
  shader_module(shader_module&& move) noexcept : shader_module_impl(std::move(move)) {
  }
  shader_module& operator=(shader_module&& move) noexcept {
    return static_cast<shader_module&>(shader_module_impl::operator=(std::move(move)));
  }
  ~shader_module() noexcept = default;
};

} // namespace pngine
} // namespace csc
