module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.framebuffer;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class framebuffer : private framebuffer_impl {
 public:
  explicit framebuffer() : framebuffer_impl() {
  }
  explicit framebuffer(
      const vk::Device& device,
      const vk::ImageView& attachment,
      const vk::RenderPass& bind_pass,
      vk::Extent2D swapchain_extent)
      : framebuffer_impl(device, attachment, bind_pass, swapchain_extent) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  framebuffer(framebuffer&& move) noexcept : framebuffer_impl(std::move(move)) {
  }
  framebuffer& operator=(framebuffer&& move) noexcept {
    return static_cast<framebuffer&>(framebuffer_impl::operator=(std::move(move)));
  }
  ~framebuffer() noexcept = default;
};

} // namespace pngine
} // namespace csc
