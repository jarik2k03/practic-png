module;
#include <utility>
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
  vk::Bool32 m_is_created = false;
  const vk::Instance* m_instance = nullptr;

 public:
  explicit debug_reportEXT_impl() = default;
  ~debug_reportEXT_impl() noexcept;
  debug_reportEXT_impl(debug_reportEXT_impl&& move) noexcept = default;
  debug_reportEXT_impl& operator=(debug_reportEXT_impl&& move) noexcept;
  explicit debug_reportEXT_impl(const vk::Instance& instance);
  void do_clear() noexcept;
};

debug_reportEXT_impl& debug_reportEXT_impl::operator=(debug_reportEXT_impl&& move) noexcept {
  [[unlikely]] if (&move == this)
    return *this;
  do_clear();
  m_debug_report = move.m_debug_report;
  m_dispatcher = move.m_dispatcher;
  m_is_created = std::exchange(move.m_is_created, false);
  m_instance = move.m_instance;
  return *this;
}

void debug_reportEXT_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_instance->destroyDebugReportCallbackEXT(m_debug_report, nullptr, m_dispatcher);
    m_is_created = false;
  }
}

debug_reportEXT_impl::~debug_reportEXT_impl() noexcept {
  do_clear();
}

debug_reportEXT_impl::debug_reportEXT_impl(const vk::Instance& instance) {
  m_dispatcher.vkCreateDebugReportCallbackEXT =
      reinterpret_cast<pngine::pfn_create>(instance.getProcAddr("vkCreateDebugReportCallbackEXT"));
  m_dispatcher.vkDestroyDebugReportCallbackEXT =
      reinterpret_cast<pngine::pfn_destroy>(instance.getProcAddr("vkDestroyDebugReportCallbackEXT"));
  using enum vk::DebugReportFlagBitsEXT;
  vk::DebugReportCallbackCreateInfoEXT description{};
  description.sType = vk::StructureType::eDebugReportCreateInfoEXT;
  description.pNext = nullptr;
  description.flags = eError | eWarning | eDebug | eInformation | ePerformanceWarning;
  description.pfnCallback = &pngine::custom_debug_report_callback;
  description.pUserData = nullptr;

  m_debug_report = instance.createDebugReportCallbackEXT(description, nullptr, m_dispatcher);
  m_instance = &instance;
  m_is_created = true;
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
