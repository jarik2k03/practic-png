#include <vulkan/vulkan.hpp>
int main() {

  vk::DebugReportCallbackEXT report;
  vk::Instance instance;
  const auto cbck = instance.destroyDebugReportCallbackEXT();
  vk::Flags<vk::DebugReportFlagBitsEXT> flags;
  auto dev = instance.enumeratePhysicalDevices();
  dev[0].getQueueFamilyProperties().at(0).queueFlags;
  dev[0].createDevice(
}
