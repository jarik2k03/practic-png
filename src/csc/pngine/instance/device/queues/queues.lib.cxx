module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.queues;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class queues : private queues_impl {
 public:
  explicit queues() : queues_impl() {
  }
  explicit queues(const vk::PhysicalDevice& queues) : queues_impl(queues) {
  }
  queues(queues&& move) noexcept : queues_impl(std::move(move)) {
  }
  queues& operator=(queues&& move) noexcept {
    return static_cast<queues&>(queues_impl::operator=(std::move(move)));
  }
  ~queues() noexcept = default;
  void clear() noexcept {
    this->do_clear();
  }
};

} // namespace pngine
} // namespace csc

