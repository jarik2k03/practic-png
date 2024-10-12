module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.render_pass;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class render_pass : private render_pass_impl {
 public:
  explicit render_pass() : render_pass_impl() {
  }
  explicit render_pass(const vk::Device& device, vk::Format swapchain_format)
      : render_pass_impl(device, swapchain_format) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  render_pass(render_pass&& move) noexcept : render_pass_impl(std::move(move)) {
  }
  render_pass& operator=(render_pass&& move) noexcept {
    return static_cast<render_pass&>(render_pass_impl::operator=(std::move(move)));
  }
  ~render_pass() noexcept = default;
};

} // namespace pngine
} // namespace csc
