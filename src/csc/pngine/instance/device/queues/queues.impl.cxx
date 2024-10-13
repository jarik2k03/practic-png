module;
#include <bits/move.h>
#include <cstdint>
module csc.pngine.instance.device.queues:impl;

export import vulkan_hpp;

import stl.vector;

namespace csc {
namespace pngine {
class queues_impl {
 private:
  uint32_t m_queues_family_index;
  std::vector<float> m_priorities;

 public:
  explicit queues_impl() = default;
  ~queues_impl() noexcept = default;
  queues_impl(queues_impl&& move) noexcept = default;
  queues_impl& operator=(queues_impl&& move) noexcept = default;
  explicit queues_impl(uint32_t q_family, const std::vector<float>& q_priorities);
  explicit queues_impl(uint32_t q_family, std::vector<float>&& q_priorities);

  vk::DeviceQueueCreateInfo do_bring_info() const;
};

queues_impl::queues_impl(uint32_t a, const std::vector<float>& b) : m_queues_family_index(a), m_priorities(b) {
}
queues_impl::queues_impl(uint32_t a, std::vector<float>&& b) : m_queues_family_index(a), m_priorities(std::move(b)) {
}

vk::DeviceQueueCreateInfo queues_impl::do_bring_info() const {
  vk::DeviceQueueCreateInfo q_description{};
  q_description.sType = vk::StructureType::eDeviceQueueCreateInfo;
  q_description.queueFamilyIndex = m_queues_family_index;
  q_description.queueCount = m_priorities.size();
  q_description.pQueuePriorities = m_priorities.data();
  return q_description;
}

} // namespace pngine
} // namespace csc
