module;
#include <cstdint>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
module csc.pngine.instance.debug_reportEXT:utility;
export import vulkan_hpp;

import stl.iostream;

namespace csc {
namespace pngine {

using pfn_create = PFN_vkCreateDebugReportCallbackEXT;
using pfn_destroy = PFN_vkDestroyDebugReportCallbackEXT;
struct debug_dispatch : public vk::DispatchLoaderBase {
  pfn_create vkCreateDebugReportCallbackEXT = 0ul;
  pfn_destroy vkDestroyDebugReportCallbackEXT = 0ul;
};

VKAPI_ATTR vk::Bool32 VKAPI_CALL custom_debug_report_callback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t /*object*/,
    size_t /*location*/,
    int32_t /*messageCode*/,
    const char* /*pLayerPrefix*/,
    const char* pMessage,
    void* /*pUserData*/) {
  const auto flags_value = static_cast<vk::DebugReportFlagsEXT>(flags);
  const auto object_type = static_cast<vk::DebugReportObjectTypeEXT>(objectType);
  std::clog << "\033[31;1;4m" << "[VALIDATION]" << "\033[0m" << ':';
  std::clog << '[' << vk::to_string(flags_value) << ']' << ':';
  std::clog << '[' << vk::to_string(object_type) << ']' << ':';
  std::clog << pMessage << '\n';
  return 0;
}
} // namespace pngine
} // namespace csc
