module;
#include <bits/stl_iterator.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
module csc.pngine.instance:impl;

import stl.vector;
import stl.string;
import stl.set;

#ifndef NDEBUG
import csc.pngine.instance.debug_reportEXT;
#endif
import csc.pngine.instance.device;

export import vulkan_hpp;

import :utility;
namespace csc {
namespace pngine {
class instance_impl {
 private:
  vk::Instance m_instance{};
  std::set<std::string>* m_vk_extensions{};
  std::vector<const char*> m_enabled_extensions{};
  std::vector<const char*> m_enabled_layers{};
  std::vector<vk::PhysicalDevice> m_phys_devices{};
  // внутренние компоненты
#ifndef NDEBUG
  pngine::debug_reportEXT m_debug_report{};
  pngine::device m_device{};
#endif
 public:
  explicit instance_impl() = default;
  ~instance_impl() noexcept = default;
  instance_impl(instance_impl&& move) noexcept = default;
  instance_impl& operator=(instance_impl&& move) noexcept = default;
  explicit instance_impl(const vk::ApplicationInfo& app_info);

#ifndef NDEBUG
  void do_create_debug_reportEXT();
#endif
  void do_create_device();
  void do_bring_physical_devices();
  vk::Instance& do_get();
  const vk::Instance& do_get() const;
};

instance_impl::instance_impl(const vk::ApplicationInfo& app_info) {
  m_vk_extensions = const_cast<std::set<std::string>*>(&vk::getInstanceExtensions());
  const char* const surface_ext = pngine::bring_surface_extension();

  m_enabled_extensions.reserve(256ul);
  [[likely]] if (surface_ext != nullptr) {
    m_enabled_extensions.emplace_back("VK_KHR_surface");
    m_enabled_extensions.emplace_back(surface_ext);
  }
#ifndef NDEBUG
  m_enabled_layers.reserve(256ul);
  m_enabled_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  m_enabled_extensions.emplace_back("VK_EXT_debug_report");
#endif

  vk::InstanceCreateInfo description{};
#ifndef NDEBUG
  using enum vk::DebugReportFlagBitsEXT;
  const auto config = pngine::pnext_debug_messenger_configuration({eError | eWarning | eDebug});
  description.setPNext(&config); // в debug - config
#else
  description.setPNext(nullptr); // в release - nullptr
#endif
  description.sType = vk::StructureType::eInstanceCreateInfo;
  description.pApplicationInfo = &app_info;
  description.enabledLayerCount = m_enabled_layers.size();
  description.ppEnabledLayerNames = m_enabled_layers.data();
  description.enabledExtensionCount = m_enabled_extensions.size();
  description.ppEnabledExtensionNames = m_enabled_extensions.data();

  m_instance = vk::createInstance(description, nullptr);
}

vk::Instance& instance_impl::do_get() {
  return m_instance;
}
const vk::Instance& instance_impl::do_get() const {
  return m_instance;
}

#ifndef NDEBUG
void instance_impl::do_create_debug_reportEXT() {
  m_debug_report = pngine::debug_reportEXT(m_instance);
}
#endif

void instance_impl::do_create_device() {
  m_device = pngine::device(m_phys_devices.front()); // для начала - пока так
}

void instance_impl::do_bring_physical_devices() {
  m_phys_devices = m_instance.enumeratePhysicalDevices();
}

} // namespace pngine
} // namespace csc
