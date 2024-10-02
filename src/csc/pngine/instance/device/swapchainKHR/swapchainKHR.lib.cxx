module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.swapchainKHR;

import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.commons.utility.queue_family_indices;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class swapchainKHR : private swapchainKHR_impl {
 public:
  explicit swapchainKHR() : swapchainKHR_impl() {
  }
  explicit swapchainKHR(const vk::Device& device, const vk::SurfaceKHR& surface, const pngine::swapchain_details& details, const pngine::queue_family_indices& indices) : swapchainKHR_impl(device, surface, details, indices) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  swapchainKHR(swapchainKHR&& move) noexcept : swapchainKHR_impl(std::move(move)) {
  }
  swapchainKHR& operator=(swapchainKHR&& move) noexcept {
    return static_cast<swapchainKHR&>(swapchainKHR_impl::operator=(std::move(move)));
  }
  ~swapchainKHR() noexcept = default;
};

} // namespace pngine
} // namespace csc
