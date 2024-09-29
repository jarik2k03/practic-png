module;
#include <utility>
#include <bits/stl_iterator.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstring>
module csc.pngine.instance:impl;

import stl.vector;
import stl.string;
import stl.set;
import stl.variant;
import stl.stdexcept;
import stl.string_view;

import csc.pngine.commons.utility.debug_reportEXT;
import csc.pngine.instance.debug_reportEXT;
import csc.pngine.instance.device;
import csc.pngine.instance.surfaceKHR;
import csc.pngine.window_handler;

export import vulkan_hpp;

import :utility;
namespace csc {
namespace pngine {
class instance_impl {
 private:
  vk::Instance m_instance{};
  const std::set<std::string>* m_vk_extensions{};
  std::vector<const char*> m_enabled_extensions{};
  std::vector<const char*> m_enabled_layers{};
  std::vector<vk::PhysicalDevice> m_phys_devices{};
  vk::Bool32 m_is_created = false;
  // внутренние компоненты
  pngine::debug_reportEXT m_debug_report{};
  pngine::device m_device{};
  pngine::surfaceKHR m_surface{};

 public:
  explicit instance_impl() = default;
  ~instance_impl() noexcept;
  instance_impl(instance_impl&& move) noexcept = default;
  instance_impl& operator=(instance_impl&& move) noexcept;
  explicit instance_impl(const vk::ApplicationInfo& app_info);

  void do_create_debug_reportEXT();
  void do_create_device(std::string_view dev_name);
  void do_create_surfaceKHR(const pngine::v_window_handler& handler);
  void do_bring_physical_devices();
  vk::Instance& do_get();
  const vk::Instance& do_get() const;
  void do_clear() noexcept;
};

instance_impl::~instance_impl() noexcept {
  do_clear();
}

instance_impl& instance_impl::operator=(instance_impl&& move) noexcept {
  [[unlikely]] if (&move == this)
    return *this;
  do_clear();
  m_instance = std::move(move.m_instance);
  m_debug_report = std::move(move.m_debug_report);
  m_device = std::move(move.m_device);
  m_phys_devices = std::move(move.m_phys_devices);
  m_vk_extensions = move.m_vk_extensions;
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_enabled_layers = std::move(move.m_enabled_layers);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

instance_impl::instance_impl(const vk::ApplicationInfo& app_info) {
  m_vk_extensions = &vk::getInstanceExtensions();
  const char* const surface_ext = pngine::bring_surface_extension();

  m_enabled_extensions.reserve(256ul);
  [[likely]] if (surface_ext != nullptr) {
    m_enabled_extensions.emplace_back("VK_KHR_surface");
    m_enabled_extensions.emplace_back(surface_ext);
  }
  if constexpr (pngine::is_debug_build()) {
    m_enabled_layers.reserve(256ul);
    m_enabled_layers.emplace_back("VK_LAYER_KHRONOS_validation");
    m_enabled_extensions.emplace_back("VK_EXT_debug_report");
  }
  vk::InstanceCreateInfo description{};
  using enum vk::DebugReportFlagBitsEXT;
  constexpr const auto config =
      pngine::pnext_debug_messenger_configuration({eError | eWarning | eDebug | ePerformanceWarning});

  description.pNext = (config.has_value()) ? (&*config) : nullptr; // в release - nullopt
  description.sType = vk::StructureType::eInstanceCreateInfo;
  description.pApplicationInfo = &app_info;
  description.enabledLayerCount = m_enabled_layers.size();
  description.ppEnabledLayerNames = m_enabled_layers.data();
  description.enabledExtensionCount = m_enabled_extensions.size();
  description.ppEnabledExtensionNames = m_enabled_extensions.data();

  m_instance = vk::createInstance(description, nullptr);
  m_is_created = true;
}

vk::Instance& instance_impl::do_get() {
  return m_instance;
}
const vk::Instance& instance_impl::do_get() const {
  return m_instance;
}

void instance_impl::do_clear() noexcept {
  if (m_is_created) {
    if constexpr (pngine::is_debug_build()) {
      m_debug_report.clear();
    }
    m_device.clear(); // удаление логического устройства
    m_surface.clear(); // удаление поверхности окна
    m_instance.destroy(); // прежде чем очищать устройство, надо очистить его вложенности
    m_is_created = false;
  }
}

void instance_impl::do_create_debug_reportEXT() {
  if constexpr (pngine::is_debug_build()) {
    m_debug_report = pngine::debug_reportEXT(m_instance);
  }
}
void instance_impl::do_create_surfaceKHR(const pngine::v_window_handler& handler) {
  auto prod_surface = std::visit(pngine::f_create_surface(m_instance), handler);
  m_surface = pngine::surfaceKHR(m_instance, prod_surface);
}

void instance_impl::do_create_device(std::string_view gpu_name) {
  [[unlikely]] if (m_phys_devices.size() == 0ul) {
    throw std::runtime_error("Не найдены видеовычислители на текущем компьютере!");
  }
  const auto is_selected_gpu = [&gpu_name](const auto& gpu) {
    return std::strcmp(gpu.getProperties().deviceName, gpu_name.data()) == 0ul;
  };
  const auto gpu_pos = std::ranges::find_if(m_phys_devices, is_selected_gpu);
  [[likely]] if (gpu_pos != m_phys_devices.cend()) { m_device = pngine::device(*gpu_pos); }
  m_device = pngine::device(m_phys_devices.front());
  // если не нашли желанную видеокарту -> используем первую попавшуюся
}

void instance_impl::do_bring_physical_devices() {
  m_phys_devices = m_instance.enumeratePhysicalDevices();
}

} // namespace pngine
} // namespace csc
