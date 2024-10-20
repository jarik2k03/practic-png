module;
#include <vulkan/vulkan_core.h>
export module csc.pngine.commons.utility.debug_reportEXT;

import vulkan_hpp;
import stl.iostream;
import stl.optional;

export namespace csc {
namespace pngine {

#ifndef NDEBUG
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
#endif

using o_VkDebugReportCallbackCreateInfoEXT = std::optional<vk::DebugReportCallbackCreateInfoEXT>;
constexpr auto pnext_debug_messenger_configuration([[maybe_unused]] vk::DebugReportFlagsEXT flags) noexcept {
#ifndef NDEBUG
  vk::DebugReportCallbackCreateInfoEXT description{};
  description.sType = vk::StructureType::eDebugReportCreateInfoEXT;
  description.pNext = nullptr;
  description.flags = flags;
  description.pfnCallback = &pngine::custom_debug_report_callback;
  description.pUserData = nullptr;
  return o_VkDebugReportCallbackCreateInfoEXT(description);
#else
  return o_VkDebugReportCallbackCreateInfoEXT(std::nullopt);
#endif
}

} // namespace pngine
} // namespace csc
