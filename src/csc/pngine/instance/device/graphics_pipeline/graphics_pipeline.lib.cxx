module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.graphics_pipeline;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class graphics_pipeline : private graphics_pipeline_impl {
 public:
  explicit graphics_pipeline() = delete;

  explicit graphics_pipeline(const vk::Device& device) : graphics_pipeline_impl(device) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  void create() {
    this->do_create();
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
