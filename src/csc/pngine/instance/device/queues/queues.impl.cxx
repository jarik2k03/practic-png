module;
#include <utility>
module csc.pngine.instance.queues:impl;

export import vulkan_hpp;

import stl.stdexcept;
import stl.vector;

import :utility;
namespace csc {
namespace pngine {
class queues_impl {
 private:
  // std::vector<vk::DeviceQueueCreateInfo>
  vk::Bool32 m_is_created = false;

 public:
  explicit queues_impl() = default;
  ~queues_impl() noexcept;
  queues_impl(queues_impl&& move) noexcept = default;
  queues_impl& operator=(queues_impl&& move) noexcept;
  explicit queues_impl(const vk::PhysicalDevice& dev);
  void do_clear() noexcept;
};

queues_impl::~queues_impl() noexcept {
  do_clear();
}
queues_impl& queues_impl::operator=(queues_impl&& move) noexcept {
  if (this == &move)
    return *this;
  do_clear();
  m_queues = move.m_queues;
  m_indices = move.m_indices;
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
queues_impl::queues_impl(const vk::PhysicalDevice& dev) {
  m_indices = pngine::bring_indices_from_phys_queues(dev);

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

  m_queues = dev.createDevice(description);
  m_is_created = true; // если кинет исключение - не будет is created
}

void queues_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_queues.destroy();
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc

