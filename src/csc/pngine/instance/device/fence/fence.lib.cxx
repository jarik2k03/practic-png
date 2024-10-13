module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.fence;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class fence : private fence_impl {
 public:
  explicit fence() : fence_impl() {
  }
  explicit fence(const vk::Device& device, vk::FenceCreateFlagBits signal = {}) : fence_impl(device, signal) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  fence(fence&& move) noexcept : fence_impl(std::move(move)) {
  }
  fence& operator=(fence&& move) noexcept {
    return static_cast<fence&>(fence_impl::operator=(std::move(move)));
  }
  ~fence() noexcept = default;
};

} // namespace pngine
} // namespace csc
