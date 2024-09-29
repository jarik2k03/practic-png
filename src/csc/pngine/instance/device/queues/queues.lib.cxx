module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.queues;
import stl.vector;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class queues : private queues_impl {
 public:
  explicit queues() : queues_impl() {
  }
  explicit queues(uint32_t q_family, const std::vector<float>& q_priorities) : queues_impl(q_family, q_priorities) {
  }
  explicit queues(uint32_t q_family, std::vector<float>&& q_priorities)
      : queues_impl(q_family, std::move(q_priorities)) {
  }
  vk::DeviceQueueCreateInfo bring_info() const {
    return this->do_bring_info();
  }
  queues(queues&& move) noexcept : queues_impl(std::move(move)) {
  }
  queues& operator=(queues&& move) noexcept {
    return static_cast<queues&>(queues_impl::operator=(std::move(move)));
  }
  ~queues() noexcept = default;
};

} // namespace pngine
} // namespace csc
