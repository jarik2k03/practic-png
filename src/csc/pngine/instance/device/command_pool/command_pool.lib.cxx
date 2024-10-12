module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.command_pool;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class command_pool : private command_pool_impl {
 public:
  explicit command_pool() : command_pool_impl() {
  }
  explicit command_pool(const vk::Device& device, uint32_t queue_family) : command_pool_impl(device, queue_family) {
  }
  auto allocate_command_buffers(vk::CommandBufferLevel level, uint32_t count) const {
    return this->do_allocate_command_buffers(level, count);
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  command_pool(command_pool&& move) noexcept : command_pool_impl(std::move(move)) {
  }
  command_pool& operator=(command_pool&& move) noexcept {
    return static_cast<command_pool&>(command_pool_impl::operator=(std::move(move)));
  }
  ~command_pool() noexcept = default;
};

} // namespace pngine
} // namespace csc
