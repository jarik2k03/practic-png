module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.pipeline_layout;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class pipeline_layout : private pipeline_layout_impl {
 public:
  explicit pipeline_layout() : pipeline_layout_impl() {
  }
  explicit pipeline_layout(const vk::Device& device) : pipeline_layout_impl(device) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  pipeline_layout(pipeline_layout&& move) noexcept : pipeline_layout_impl(std::move(move)) {
  }
  pipeline_layout& operator=(pipeline_layout&& move) noexcept {
    return static_cast<pipeline_layout&>(pipeline_layout_impl::operator=(std::move(move)));
  }
  ~pipeline_layout() noexcept = default;
};

} // namespace pngine
} // namespace csc
