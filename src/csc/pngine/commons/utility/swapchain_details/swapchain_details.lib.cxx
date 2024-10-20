module;
#include <vulkan/vulkan_core.h>
export module csc.pngine.commons.utility.swapchain_details;

import stl.vector;

import vulkan_hpp;

export namespace csc {
namespace pngine {
using swapchain_createKHR = PFN_vkCreateSwapchainKHR;
using swapchain_destroyKHR = PFN_vkDestroySwapchainKHR;
struct swapchain_dispatch : public vk::DispatchLoaderBase {
  swapchain_createKHR vkCreateSwapchainKHR = 0ul;
  swapchain_destroyKHR vkDestroySwapchainKHR = 0ul;
};

struct swapchain_details {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> present_modes;
  vk::Extent2D window_framebuffer;
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
