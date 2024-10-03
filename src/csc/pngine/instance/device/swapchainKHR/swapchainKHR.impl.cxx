module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.swapchainKHR:impl;

import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.commons.utility.queue_family_indices;
export import vulkan_hpp;

import stl.stdexcept;
import stl.vector;

import :utility;
namespace csc {
namespace pngine {
class swapchainKHR_impl {
 private:
  const vk::Device* m_keep_device = nullptr;
  const vk::SurfaceKHR* m_keep_surface = nullptr;

  std::vector<vk::Image> m_images{};
  vk::SwapchainKHR m_swapchainKHR{};
  vk::Format m_image_format{};
  vk::Extent2D m_extent{};
  vk::Bool32 m_is_created = false;

 public:
  explicit swapchainKHR_impl() = default;
  explicit swapchainKHR_impl(
      const vk::Device& device,
      const vk::SurfaceKHR& surface,
      const pngine::swapchain_details& details,
      const pngine::queue_family_indices& indices);
  ~swapchainKHR_impl() noexcept = default;
  swapchainKHR_impl(swapchainKHR_impl&& move) noexcept = default;
  swapchainKHR_impl& operator=(swapchainKHR_impl&& move) noexcept;
  const vk::Format& do_get_image_format() const;
  const std::vector<vk::Image>& do_get_images() const;
  void do_clear() noexcept;
};

swapchainKHR_impl& swapchainKHR_impl::operator=(swapchainKHR_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_swapchainKHR = move.m_swapchainKHR;
  m_keep_device = move.m_keep_device;
  m_keep_surface = move.m_keep_surface;
  m_extent = move.m_extent;
  m_image_format = move.m_image_format;
  m_images = std::move(move.m_images);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

swapchainKHR_impl::swapchainKHR_impl(
    const vk::Device& device,
    const vk::SurfaceKHR& surface,
    const pngine::swapchain_details& details,
    const pngine::queue_family_indices& indices)
    : m_keep_device(&device), m_keep_surface(&surface) {
  [[unlikely]] if (details.formats.empty() || details.present_modes.empty())
    throw std::runtime_error(
        "Device:SwapchainKHR: невозможно создать swapchainKHR, отсутствует SurfaceFormats или PresentModes!");
  const auto image_count = pngine::choose_image_count(details.capabilities, details.capabilities.minImageCount);

  vk::SwapchainCreateInfoKHR description{};
  description.sType = vk::StructureType::eSwapchainCreateInfoKHR;
  description.pNext = nullptr;
  m_extent = pngine::choose_extent(details.capabilities);

  description.imageExtent = m_extent;
  const auto surf_format = pngine::choose_surface_format(details.formats);
  description.imageFormat = surf_format.format;
  description.imageColorSpace = surf_format.colorSpace;
  description.presentMode = pngine::choose_present_mode(details.present_modes);
  description.surface = surface;
  description.minImageCount = image_count;
  description.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
  description.imageArrayLayers = 1u;

  const uint32_t queue_family_indices[] = {indices.graphics.value(), indices.present.value()};
  if (indices.graphics.value() != indices.present.value()) {
    description.imageSharingMode = vk::SharingMode::eConcurrent;
    description.queueFamilyIndexCount = 2u;
    description.pQueueFamilyIndices = queue_family_indices;
  } else {
    description.imageSharingMode = vk::SharingMode::eExclusive;
    description.queueFamilyIndexCount = 0u;
    description.pQueueFamilyIndices = nullptr;
  }
  description.preTransform = details.capabilities.currentTransform;
  description.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  description.clipped = vk::True;
  description.oldSwapchain = vk::SwapchainKHR{};

  m_swapchainKHR = m_keep_device->createSwapchainKHR(description, nullptr);
  m_images = m_keep_device->getSwapchainImagesKHR(m_swapchainKHR);
  m_image_format = surf_format.format;

  m_is_created = true;
}

void swapchainKHR_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroySwapchainKHR(m_swapchainKHR, nullptr);
    m_is_created = false;
  }
}

auto swapchainKHR_impl::do_get_images() const -> const decltype(m_images)& {
  return m_images;
}

auto swapchainKHR_impl::do_get_image_format() const -> const decltype(m_image_format)& {
  return m_image_format;
}

} // namespace pngine
} // namespace csc
