#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_xcb.h>
int main() {
  struct dispatch_t {};
  dispatch_t dispatch;
  vk::DebugReportCallbackEXT report;
  vk::InstanceCreateInfo instinf;
  vk::Instance instance = vk::createInstance(instinf, nullptr, {});
  vk::Flags<vk::DebugReportFlagBitsEXT> flags;
  auto dev = instance.enumeratePhysicalDevices();
  dev[0].getQueueFamilyProperties().at(0).queueFlags;
  vk::DeviceCreateInfo devinf;
  vk::Device device = dev[0].createDevice(devinf);
  vk::SurfaceKHR surface;
  dev[0].getSurfaceSupportKHR(0, surface);
  const auto families = dev[0u].getQueueFamilyProperties();
  const auto caps = dev[0].getSurfaceCapabilitiesKHR(surface, dispatch);
  const auto formats = dev[0].getSurfaceFormatsKHR(surface,dispatch);
  const auto modes = dev[0].getSurfacePresentModesKHR(surface, dispatch);
  vk::SwapchainCreateInfoKHR swapchaininfo;
  vk::StructureType::eSwapchainCreateInfoKHR;
  vk::SurfaceCapabilitiesKHR caps;
}
