module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.semaphore;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class semaphore : private semaphore_impl {
 public:
  explicit semaphore() : semaphore_impl() {
  }
  explicit semaphore(const vk::Device& device) : semaphore_impl(device) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  semaphore(semaphore&& move) noexcept : semaphore_impl(std::move(move)) {
  }
  semaphore& operator=(semaphore&& move) noexcept {
    return static_cast<semaphore&>(semaphore_impl::operator=(std::move(move)));
  }
  ~semaphore() noexcept = default;
};

} // namespace pngine
} // namespace csc
