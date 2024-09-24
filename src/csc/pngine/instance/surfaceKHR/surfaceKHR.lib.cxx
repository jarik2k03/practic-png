module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.surfaceKHR;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class surfaceKHR : private surfaceKHR_impl {
 public:
  explicit surfaceKHR() : surfaceKHR_impl() {
  }
  explicit surfaceKHR(const vk::Instance& instance) : surfaceKHR_impl(instance) {
  }
  surfaceKHR(surfaceKHR&& move) noexcept : surfaceKHR_impl(std::move(move)) {
  }
  surfaceKHR& operator=(surfaceKHR&& move) noexcept {
    return static_cast<surfaceKHR&>(surfaceKHR_impl::operator=(std::move(move)));
  }
  ~surfaceKHR() noexcept = default;
  void clear() noexcept {
    this->do_clear();
  }
};

} // namespace pngine
} // namespace csc
