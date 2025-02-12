module;
#include <utility>
#include <bits/stl_iterator.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstring>
export module csc.pngine.instance;

import stl.vector;
import stl.string;
import stl.set;
import stl.stdexcept;
import stl.optional;
import stl.string_view;

import csc.pngine.commons.utility.debug_reportEXT;
import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.instance.debug_reportEXT;
import csc.pngine.instance.device;
import csc.pngine.instance.surfaceKHR;
import csc.pngine.window_handler;

import vulkan_hpp;

namespace csc {
namespace pngine {

consteval bool is_debug_build() noexcept {
#ifndef NDEBUG
  return true;
#else
  return false;
#endif
}

const char* bring_surface_extension() noexcept {
#ifdef _WIN32
  return "VK_KHR_win32_surface";
#else
  const char* const session_ty = ::secure_getenv("XDG_SESSION_TYPE");
  if (session_ty == nullptr) {
    return nullptr;
  }
  const std::string session_type(session_ty);
  if (session_type == "x11") {
    return "VK_KHR_xcb_surface";
  } else if (session_type == "wayland") {
    return "VK_KHR_wayland_surface";
  } else {
    return nullptr;
  }
#endif
  return nullptr;
}

} // namespace pngine
} // namespace csc

export namespace csc {
namespace pngine {
class instance {
 private:
  vk::Instance m_instance{};
  const std::set<std::string>* m_vk_extensions{};
  std::vector<const char*> m_enabled_extensions{};
  std::vector<const char*> m_enabled_layers{};
  std::vector<vk::PhysicalDevice> m_phys_devices{};
  vk::Bool32 m_is_created = false;
  pngine::swapchain_dispatch m_swapchain_dispatch{};
  // внутренние компоненты
  pngine::debug_reportEXT m_debug_report{};
  pngine::device m_device{};
  pngine::surfaceKHR m_surface{};

 public:
  explicit instance() = default;
  ~instance() noexcept;
  instance(instance&& move) noexcept = default;
  instance& operator=(instance&& move) noexcept;
  explicit instance(const vk::ApplicationInfo& app_info);

  void create_debug_reportEXT();
  pngine::device& create_device(std::string_view dev_name);
  void create_surfaceKHR(const pngine::window_handler& handler);
  void bring_physical_devices();
  pngine::device& get_device();
  pngine::swapchain_dispatch get_swapchainKHR_dispatch() const noexcept;
  vk::Instance get() const noexcept;
  void clear() noexcept;
};

instance::~instance() noexcept {
  clear();
}

instance& instance::operator=(instance&& move) noexcept {
  [[unlikely]] if (&move == this)
    return *this;
  clear();
  m_swapchain_dispatch = move.m_swapchain_dispatch;

  m_instance = std::move(move.m_instance);
  m_debug_report = std::move(move.m_debug_report);
  m_device = std::move(move.m_device);
  m_surface = std::move(move.m_surface);
  m_phys_devices = std::move(move.m_phys_devices);
  m_vk_extensions = move.m_vk_extensions;
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_enabled_layers = std::move(move.m_enabled_layers);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

instance::instance(const vk::ApplicationInfo& app_info) {
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

  description.pNext = (config.has_value()) ? (&config.value()) : nullptr; // в release - nullopt
  description.sType = vk::StructureType::eInstanceCreateInfo;
  description.pApplicationInfo = &app_info;
  description.enabledLayerCount = m_enabled_layers.size();
  description.ppEnabledLayerNames = m_enabled_layers.data();
  description.enabledExtensionCount = m_enabled_extensions.size();
  description.ppEnabledExtensionNames = m_enabled_extensions.data();
  m_instance = vk::createInstance(description, nullptr);

  m_swapchain_dispatch.vkCreateSwapchainKHR =
      reinterpret_cast<swapchain_createKHR>(m_instance.getProcAddr("vkCreateSwapchainKHR"));
  m_swapchain_dispatch.vkDestroySwapchainKHR =
      reinterpret_cast<swapchain_destroyKHR>(m_instance.getProcAddr("vkDestroySwapchainKHR"));

  m_is_created = true;
}

vk::Instance instance::get() const noexcept {
  return m_instance;
}
pngine::swapchain_dispatch instance::get_swapchainKHR_dispatch() const noexcept {
  return m_swapchain_dispatch;
}

pngine::device& instance::get_device() {
  return m_device;
}
void instance::clear() noexcept {
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

void instance::create_debug_reportEXT() {
  if constexpr (pngine::is_debug_build()) {
    m_debug_report = pngine::debug_reportEXT(m_instance);
  }
}
void instance::create_surfaceKHR(const pngine::window_handler& handler) {
  m_surface = pngine::surfaceKHR(m_instance, handler.create_surface(m_instance));
}

pngine::device& instance::create_device(std::string_view gpu_name) {
  [[unlikely]] if (m_phys_devices.size() == 0ul) {
    throw std::runtime_error("Не найдены видеовычислители на текущем компьютере!");
  }
  const auto is_selected_gpu = [&gpu_name](const auto& gpu) {
    return std::strcmp(gpu.getProperties().deviceName, gpu_name.data()) == 0ul;
  };
  const auto gpu_pos = std::ranges::find_if(m_phys_devices, is_selected_gpu);

  [[likely]] if (gpu_pos != m_phys_devices.cend()) { m_device = pngine::device(*gpu_pos, m_surface.get()); }
  m_device = pngine::device(m_phys_devices.front(), m_surface.get());
  // если не нашли желанную видеокарту -> используем первую попавшуюся
  return m_device;
}

void instance::bring_physical_devices() {
  m_phys_devices = m_instance.enumeratePhysicalDevices();
}

} // namespace pngine
} // namespace csc
