module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.graphics_pipeline;

import csc.pngine.commons.utility.graphics_pipeline;
import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class graphics_pipeline : private graphics_pipeline_impl {
 public:
  explicit graphics_pipeline() = delete;

  template <pngine::c_graphics_pipeline_config Config>
  explicit graphics_pipeline(const vk::Device& device, const vk::PipelineLayout& layout, Config&& config)
      : graphics_pipeline_impl(device, layout, std::forward<Config>(config)) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  graphics_pipeline(graphics_pipeline&& move) noexcept : graphics_pipeline_impl(std::move(move)) {
  }
  graphics_pipeline& operator=(graphics_pipeline&& move) noexcept {
    return static_cast<graphics_pipeline&>(graphics_pipeline_impl::operator=(std::move(move)));
  }
  ~graphics_pipeline() noexcept = default;
};

} // namespace pngine
} // namespace csc
