module;
#include <cstdint>
module csc.pngine.instance.debug_reportEXT:impl;

import stl.iostream;
export import vulkan_hpp;

import :utility;
namespace csc {
namespace pngine {
class debug_reportEXT_impl {
 private:
  pngine::debug_dispatch m_dispatcher{};
  vk::DebugReportCallbackEXT m_debug_report{};

 public:
  explicit debug_reportEXT_impl() = default;
  ~debug_reportEXT_impl() noexcept = default;
  debug_reportEXT_impl(debug_reportEXT_impl&& move) noexcept = default;
  debug_reportEXT_impl& operator=(debug_reportEXT_impl&& move) noexcept = default;
  explicit debug_reportEXT_impl(const vk::Instance& instance);
};

debug_reportEXT_impl::debug_reportEXT_impl(const vk::Instance& instance) {
  m_dispatcher.vkCreateDebugReportCallbackEXT =
      reinterpret_cast<pngine::pfn_create>(instance.getProcAddr("vkCreateDebugReportCallbackEXT"));
  m_dispatcher.vkDestroyDebugReportCallbackEXT =
      reinterpret_cast<pngine::pfn_destroy>(instance.getProcAddr("vkDestroyDebugReportCallbackEXT"));
  using enum vk::DebugReportFlagBitsEXT;
  vk::DebugReportCallbackCreateInfoEXT description{};
  description.sType = vk::StructureType::eDebugReportCreateInfoEXT;
  description.pNext = nullptr;
  description.flags = eError | eWarning | eDebug | eInformation;
  description.pfnCallback = &pngine::custom_debug_report_callback;
  description.pUserData = nullptr;

  m_debug_report = instance.createDebugReportCallbackEXT(description, nullptr, m_dispatcher);
}

vk::DebugReportCallbackCreateInfoEXT pnext_debug_messenger_configuration_impl(vk::DebugReportFlagsEXT flags) noexcept {
  vk::DebugReportCallbackCreateInfoEXT description{};
  description.sType = vk::StructureType::eDebugReportCreateInfoEXT;
  description.pNext = nullptr;
  description.flags = flags;
  description.pfnCallback = &pngine::custom_debug_report_callback;
  description.pUserData = nullptr;
  return description;
}

} // namespace pngine
} // namespace csc
