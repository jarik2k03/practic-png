module;
export module csc.pngine.commons.utility.swapchain_details;

import stl.vector;

import vulkan_hpp;

export namespace csc {
namespace pngine {

struct swapchain_details {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> present_modes;
};

pngine::swapchain_details bring_swapchain_details_from_phys_device(
    const vk::PhysicalDevice& dev,
    const vk::SurfaceKHR& surface) {
  swapchain_details details;

  details.formats = dev.getSurfaceFormatsKHR(surface);
  details.capabilities = dev.getSurfaceCapabilitiesKHR(surface);
  details.present_modes = dev.getSurfacePresentModesKHR(surface);
  return details;
}

} // namespace pngine
} // namespace csc

