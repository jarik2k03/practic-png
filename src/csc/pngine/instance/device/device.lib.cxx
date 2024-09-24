module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class device : private device_impl {
 public:
  explicit device() : device_impl() {
  }
  explicit device(const vk::PhysicalDevice& device) : device_impl(device) {
  }
  device(device&& move) noexcept : device_impl(std::move(move)) {
  }
  device& operator=(device&& move) noexcept {
    return static_cast<device&>(device_impl::operator=(std::move(move)));
  }
  ~device() noexcept = default;
  void clear() noexcept {
    this->do_clear();
  }
};

} // namespace pngine
} // namespace csc
