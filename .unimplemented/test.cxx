#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_xcb.h>
int main() {

  vk::DebugReportCallbackEXT report;
  vk::InstanceCreateInfo instinf;
  vk::Instance instance = vk::createInstance(instinf, {}, {});
  vk::Flags<vk::DebugReportFlagBitsEXT> flags;
  auto dev = instance.enumeratePhysicalDevices();
  dev[0].getQueueFamilyProperties().at(0).queueFlags;
  vk::DeviceCreateInfo devinf;
  vk::Device device = dev[0].createDevice(devinf);
}
