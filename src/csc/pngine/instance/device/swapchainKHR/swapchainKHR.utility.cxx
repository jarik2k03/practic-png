module;
#include <bits/stdint-uintn.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

module csc.pngine.instance.device.swapchainKHR:utility;
export import vulkan_hpp;
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
