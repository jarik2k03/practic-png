module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.swapchainKHR:impl;

import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.commons.forward.device;

export import vulkan_hpp;

import stl.stdexcept;

import :utility;
namespace csc {
namespace pngine {
class swapchainKHR_impl {
 private:
  const vk::Device* m_keep_device = nullptr;
  const vk::SurfaceKHR* m_keep_surface = nullptr;

  vk::SwapchainKHR m_swapchainKHR{};
  vk::Bool32 m_is_created = false;

 public:
  explicit swapchainKHR_impl() = default;
  explicit swapchainKHR_impl(const vk::Device& device, const vk::SurfaceKHR& surface, const pngine::swapchain_details& details);
  ~swapchainKHR_impl() noexcept = default;
  swapchainKHR_impl(swapchainKHR_impl&& move) noexcept = default;
  swapchainKHR_impl& operator=(swapchainKHR_impl&& move) noexcept;
  void do_clear() noexcept;
};

swapchainKHR_impl& swapchainKHR_impl::operator=(swapchainKHR_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_swapchainKHR = move.m_swapchainKHR;
  m_keep_device = move.m_keep_device;
  m_keep_surface = move.m_keep_surface;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

swapchainKHR_impl::swapchainKHR_impl(const vk::Device& device, const vk::SurfaceKHR& surface, const pngine::swapchain_details& details)
    : m_keep_device(&device), m_keep_surface(&surface) {
  [[unlikely]] if (details.formats.empty() || details.present_modes.empty())
    throw std::runtime_error(
        "Device:SwapchainKHR: невозможно создать swapchainKHR, отсутствует SurfaceFormats или PresentModes!");
  const auto image_count = pngine::choose_image_count(details.capabilities, details.capabilities.minImageCount);

  vk::SwapchainCreateInfoKHR description{};
  description.sType = vk::StructureType::eSwapchainCreateInfoKHR;
  description.pNext = nullptr;
  description.imageExtent = pngine::choose_extent(details.capabilities);
  const auto surf_format = pngine::choose_surface_format(details.formats);
  description.imageFormat = surf_format.format;
  description.imageColorSpace = surf_format.colorSpace;
  description.presentMode = pngine::choose_present_mode(details.present_modes);
  description.surface = surface;
  description.minImageCount = image_count;
  description.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
  description.imageArrayLayers = 1;


  m_swapchainKHR = m_keep_device->createSwapchainKHR(description, nullptr);
  m_is_created = true;
}

void swapchainKHR_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroySwapchainKHR(m_swapchainKHR, nullptr);
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
