module;
#include <utility>
module csc.pngine.instance.device:impl;

export import vulkan_hpp;

import stl.stdexcept;
import stl.vector;

import :utility;
namespace csc {
namespace pngine {
class device_impl {
 private:
  vk::Device m_device{};
  pngine::queue_family_indices m_indices{};

  std::vector<const char*> m_enabled_extensions{};
  vk::Bool32 m_is_created = false;
  // внутренние компоненты
 public:
  explicit device_impl() = default;
  ~device_impl() noexcept;
  device_impl(device_impl&& move) noexcept = default;
  device_impl& operator=(device_impl&& move) noexcept;
  explicit device_impl(const vk::PhysicalDevice& dev);
};

device_impl::~device_impl() noexcept {
  if (m_is_created != false)
    m_device.destroy();
}
device_impl& device_impl::operator=(device_impl&& move) noexcept {
  if (this == &move)
    return *this;
  if (m_is_created != false)
    m_device.destroy();
  m_device = move.m_device;
  m_indices = move.m_indices;
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
device_impl::device_impl(const vk::PhysicalDevice& dev) {
  m_indices = pngine::bring_indices_from_phys_device(dev);

  m_enabled_extensions.reserve(256ul);
  m_enabled_extensions.emplace_back("VK_KHR_swapchain");

  if (!m_indices.graphics.has_value())
    throw std::runtime_error("Отсутствует графические очереди для устройства!");
  const float priorities = 1.0f; // пока что обойдёмся без приоритетов
  vk::DeviceQueueCreateInfo q_description{};
  q_description.sType = vk::StructureType::eDeviceQueueCreateInfo;
  q_description.queueFamilyIndex = *m_indices.graphics;
  q_description.queueCount = 1;
  q_description.pQueuePriorities = &priorities;
  // очередь пока только одна (зафиксировано)
  vk::DeviceCreateInfo description{};
  description.sType = vk::StructureType::eDeviceCreateInfo;
  description.ppEnabledExtensionNames = m_enabled_extensions.data();
  description.enabledExtensionCount = m_enabled_extensions.size();
  description.pQueueCreateInfos = &q_description;
  description.queueCreateInfoCount = 1;

  m_device = dev.createDevice(description);
  m_is_created = true; // если кинет исключение - не будет is created
}

} // namespace pngine
} // namespace csc
