#include <vulkan/vulkan.hpp>
int main() {

  vk::DebugReportCallbackEXT report;
  vk::InstanceCreateInfo instinf;
  vk::Instance instance = vk::createInstance(&instinf, {}, {});
  vk::Flags<vk::DebugReportFlagBitsEXT> flags;
  auto dev = instance.enumeratePhysicalDevices();
  dev[0].getQueueFamilyProperties().at(0).queueFlags;
//   dev[0].getProperties().
}
