#include <vulkan/vulkan.hpp>

int main() {

  vk::DebugReportCallbackEXT report;
  vk::Instance instance;
  const auto cbck = instance.destroyDebugReportCallbackEXT();
  vk::Flags<vk::DebugReportFlagBitsEXT> flags;
  vk::DebugReportFlagBitsEXT::
}
