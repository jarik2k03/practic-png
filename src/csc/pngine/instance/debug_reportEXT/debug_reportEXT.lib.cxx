module;
#include <vulkan/vulkan_core.h>
#include <utility>
export module csc.pngine.instance.debug_reportEXT;

import csc.pngine.commons.utility.debug_reportEXT;
import stl.iostream;
import vulkan_hpp;

namespace csc {
namespace pngine {

using pfn_create = PFN_vkCreateDebugReportCallbackEXT;
using pfn_destroy = PFN_vkDestroyDebugReportCallbackEXT;
struct debug_dispatch : public vk::detail::DispatchLoaderBase {
  pfn_create vkCreateDebugReportCallbackEXT = 0ul;
  pfn_destroy vkDestroyDebugReportCallbackEXT = 0ul;
};
} // namespace pngine
} // namespace csc

export namespace csc {
namespace pngine {
class debug_reportEXT {
#ifndef NDEBUG
 private:
  pngine::debug_dispatch m_dispatcher{};
  vk::DebugReportCallbackEXT m_debug_report{};
  vk::Bool32 m_is_created = false;
  const vk::Instance* m_instance = nullptr;
#endif
 public:
  explicit debug_reportEXT() = default;
  ~debug_reportEXT() noexcept;
  debug_reportEXT(debug_reportEXT&& move) noexcept = default;
  debug_reportEXT& operator=(debug_reportEXT&& move) noexcept;
  explicit debug_reportEXT(const vk::Instance& instance);
  void clear() noexcept;
};

debug_reportEXT& debug_reportEXT::operator=([[maybe_unused]] debug_reportEXT&& move) noexcept {
#ifndef NDEBUG
  [[unlikely]] if (&move == this)
    return *this;
  clear();
  m_debug_report = move.m_debug_report;
  m_dispatcher = move.m_dispatcher;
  m_is_created = std::exchange(move.m_is_created, false);
  m_instance = move.m_instance;
#endif
  return *this;
}

void debug_reportEXT::clear() noexcept {
#ifndef NDEBUG
  if (m_is_created != false) {
    m_instance->destroyDebugReportCallbackEXT(m_debug_report, nullptr, m_dispatcher);
    m_is_created = false;
  }
#endif
}

debug_reportEXT::~debug_reportEXT() noexcept {
#ifndef NDEBUG
  clear();
#endif
}

debug_reportEXT::debug_reportEXT([[maybe_unused]] const vk::Instance& instance) {
#ifndef NDEBUG
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
#endif
}

} // namespace pngine
} // namespace csc
