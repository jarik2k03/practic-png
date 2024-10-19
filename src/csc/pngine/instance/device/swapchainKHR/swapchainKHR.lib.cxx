module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
#include <bits/stdint-uintn.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
export module csc.pngine.instance.device.swapchainKHR;

import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.commons.utility.queue_family_indices;
import vulkan_hpp;

import stl.stdexcept;
import stl.vector;

namespace csc {
namespace pngine {

vk::SurfaceFormatKHR choose_surface_format(const std::vector<vk::SurfaceFormatKHR>& candidates) {
  auto is_good_candidat = [](const auto& candidat) -> bool {
    return candidat.format == vk::Format::eB8G8R8A8Srgb && candidat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
  };
  const auto good_candidat_pos = std::ranges::find_if(candidates, is_good_candidat);
  return (good_candidat_pos != candidates.cend()) ? *good_candidat_pos : candidates.front();
}

vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& candidates) {
  auto is_good_candidat = [](const auto& candidat) -> bool { return candidat == vk::PresentModeKHR::eMailbox; };
  const auto good_candidat_pos = std::ranges::find_if(candidates, is_good_candidat);
  return (good_candidat_pos != candidates.cend()) ? *good_candidat_pos : vk::PresentModeKHR::eFifo;
}

vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
    vk::Extent2D new_extent{1280u, 720u}; // костыльное говно. Но протаскивать window_handler долго...
    const auto &min = capabilities.minImageExtent, &max = capabilities.maxImageExtent;
    new_extent.width = std::clamp(new_extent.width, min.width, max.width);
    new_extent.height = std::clamp(new_extent.height, min.height, max.height);
    return new_extent;
  } else {
    return capabilities.currentExtent;
  }
}

constexpr uint32_t choose_image_count(const vk::SurfaceCapabilitiesKHR& caps, uint32_t count) noexcept {
  return (caps.maxImageCount > 0) ? std::clamp(count, caps.minImageCount, caps.maxImageCount) : count;
}

} // namespace pngine
} // namespace csc

export namespace csc {
namespace pngine {
class swapchainKHR {
 private:
  const vk::Device* m_keep_device = nullptr;

  std::vector<vk::Image> m_images{};
  vk::SwapchainKHR m_swapchainKHR{};
  vk::Format m_image_format{};
  vk::Extent2D m_extent{};
  pngine::swapchain_dispatch m_dispatch{};
  vk::Bool32 m_is_created = false;

 public:
  explicit swapchainKHR() = default;
  explicit swapchainKHR(
      const vk::Device& device,
      const vk::SurfaceKHR& surface,
      const pngine::swapchain_details& details,
      const pngine::queue_family_indices& indices,
      pngine::swapchain_dispatch dispatch);
  ~swapchainKHR() noexcept;
  swapchainKHR(swapchainKHR&& move) noexcept;
  swapchainKHR& operator=(swapchainKHR&& move) noexcept;
  const vk::Format& get_image_format() const;
  const std::vector<vk::Image>& get_images() const;
  vk::Extent2D get_extent() const;
  void clear() noexcept;
  vk::SwapchainKHR get() const noexcept;
};

swapchainKHR::~swapchainKHR() noexcept {
  clear();
}

swapchainKHR::swapchainKHR(swapchainKHR&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_images(std::move(move.m_images)),
      m_swapchainKHR(move.m_swapchainKHR),
      m_image_format(move.m_image_format),
      m_extent(move.m_extent),
      m_dispatch(move.m_dispatch),
      m_is_created(std::exchange(move.m_is_created, false)) {
}

swapchainKHR& swapchainKHR::operator=(swapchainKHR&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_swapchainKHR = move.m_swapchainKHR;
  m_keep_device = move.m_keep_device;
  m_extent = move.m_extent;
  m_image_format = move.m_image_format;
  m_dispatch = move.m_dispatch;
  m_images = std::move(move.m_images);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

swapchainKHR::swapchainKHR(
    const vk::Device& device,
    const vk::SurfaceKHR& surface,
    const pngine::swapchain_details& details,
    const pngine::queue_family_indices& indices,
    pngine::swapchain_dispatch dispatch)
    : m_keep_device(&device), m_dispatch(dispatch) {
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

  m_swapchainKHR = m_keep_device->createSwapchainKHR(description, nullptr, m_dispatch);
  m_images = m_keep_device->getSwapchainImagesKHR(m_swapchainKHR);
  m_image_format = surf_format.format;

  m_is_created = true;
}

vk::SwapchainKHR swapchainKHR::get() const noexcept {
  return m_swapchainKHR;
}

void swapchainKHR::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroySwapchainKHR(m_swapchainKHR, nullptr, m_dispatch);
    m_images.clear();
    m_is_created = false;
  }
}
auto swapchainKHR::get_extent() const -> decltype(m_extent) {
  return m_extent;
}

auto swapchainKHR::get_images() const -> const decltype(m_images)& {
  return m_images;
}

auto swapchainKHR::get_image_format() const -> const decltype(m_image_format)& {
  return m_image_format;
}

} // namespace pngine
} // namespace csc
