module;
#include <utility>
module csc.pngine.instance.device:impl;

export import vulkan_hpp;

import stl.stdexcept;
import stl.vector;
import stl.optional;

import csc.pngine.instance.device.queues;

import :utility;
namespace csc {
namespace pngine {
class device_impl {
 private:
  vk::Device m_device{};
  pngine::queue_family_indices m_indices{};

  std::vector<const char*> m_enabled_extensions{};
  vk::Bool32 m_is_created = false;

 public:
  explicit device_impl() = default;
  ~device_impl() noexcept;
  device_impl(device_impl&& move) noexcept = default;
  device_impl& operator=(device_impl&& move) noexcept;
  explicit device_impl(const vk::PhysicalDevice& dev, const std::optional<vk::SurfaceKHR>& surface);
  void do_clear() noexcept;
};

device_impl::~device_impl() noexcept {
  do_clear();
}
device_impl& device_impl::operator=(device_impl&& move) noexcept {
  if (this == &move)
    return *this;
  do_clear();
  m_device = move.m_device;
  m_indices = move.m_indices;
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
device_impl::device_impl(const vk::PhysicalDevice& dev, const std::optional<vk::SurfaceKHR>& surface) {
  if (surface.has_value())
    m_indices = pngine::bring_indices_from_phys_device(dev, surface.value());
  else
    m_indices = pngine::bring_indices_from_phys_device(dev);

  m_enabled_extensions.reserve(256ul);
  m_enabled_extensions.emplace_back("VK_KHR_swapchain");

  if (!m_indices.graphics.has_value())
    throw std::runtime_error("Device: Не найдено graphics-семейство очередей для устройства!");
  if (surface.has_value() && !m_indices.present.has_value()) {
    throw std::runtime_error("Device: Не найдено present-семейство очередей для поверхности окна!");
  }

  std::vector<vk::DeviceQueueCreateInfo> q_descriptions;
  pngine::queues graphics_queues(m_indices.graphics.value(), {1.0});
  q_descriptions.emplace_back(graphics_queues.bring_info());
  if (surface.has_value() && m_indices.graphics.value() != m_indices.present.value()) {
    pngine::queues present_queues(m_indices.present.value(), {1.0}); // если индексы разные - то и очереди тоже
    q_descriptions.emplace_back(present_queues.bring_info());
  }

  vk::DeviceCreateInfo description{};
  description.sType = vk::StructureType::eDeviceCreateInfo;
  description.pNext = nullptr;
  description.ppEnabledExtensionNames = m_enabled_extensions.data();
  description.enabledExtensionCount = m_enabled_extensions.size();
  description.pQueueCreateInfos = q_descriptions.data();
  description.queueCreateInfoCount = q_descriptions.size();

  m_device = dev.createDevice(description);
  m_is_created = true; // если кинет исключение - не будет is created
}

void device_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_device.destroy();
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
