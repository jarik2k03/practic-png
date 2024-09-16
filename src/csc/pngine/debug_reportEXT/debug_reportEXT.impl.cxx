module;
#include <cstdint>
module csc.pngine.debug_reportEXT:impl;

#ifndef NDEBUG
import stl.iostream;
#endif
export import vulkan_hpp;

import :utility;
namespace csc {
namespace pngine {
class debug_reportEXT_impl {
 private:
  vk::DebugReportCallbackEXT m_debug_report{};
  vk::DebugReportCallbackCreateInfoEXT m_description{};

 public:
  explicit debug_reportEXT_impl() = default;
  ~debug_reportEXT_impl() = default;
  debug_reportEXT_impl(debug_reportEXT_impl&& move) noexcept = default;
  debug_reportEXT_impl& operator=(debug_reportEXT_impl&& move) noexcept = default;
  explicit debug_reportEXT_impl(vk::Instance& instance);
};

debug_reportEXT_impl::debug_reportEXT_impl(vk::Instance& instance) {
  using enum vk::DebugReportFlagBitsEXT;
  m_description.sType = vk::StructureType::eDebugReportCreateInfoEXT;
  m_description.pNext = nullptr;
  m_description.flags = eError | eWarning | eDebug;
  // m_description.pfnCallback = &pngine::custom_debug_report_callback;
  m_description.pUserData = nullptr;
  // vk::DispatchLoaderDynamic dld(vk::getProcAddr);
  // PFN_vkVoidFunction pfn = instance.getProcAddr("vkCreateDebugReportCallbackEXT");
  // auto* pfn_createDebugReportCallbackEXT = reinterpret_cast<decltype(&pfn)>(&pfn);
  // m_debug_report = pfn_createDebugReportCallbackEXT(&m_description, nullptr);
}

} // namespace pngine
} // namespace csc
