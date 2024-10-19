module;
#include <cstdint>
#include <bits/move.h>
export module csc.pngine.instance.device.queues;

import vulkan_hpp;

import stl.vector;

export namespace csc {
namespace pngine {
class queues {
 private:
  uint32_t m_queues_family_index{};
  std::vector<float> m_priorities;

 public:
  explicit queues() = default;
  ~queues() noexcept = default;
  queues(queues&& move) noexcept = default;
  queues& operator=(queues&& move) noexcept = default;
  explicit queues(uint32_t q_family, const std::vector<float>& q_priorities);
  explicit queues(uint32_t q_family, std::vector<float>&& q_priorities);

  vk::DeviceQueueCreateInfo bring_info() const;
};

queues::queues(uint32_t a, const std::vector<float>& b) : m_queues_family_index(a), m_priorities(b) {
}
queues::queues(uint32_t a, std::vector<float>&& b) : m_queues_family_index(a), m_priorities(std::move(b)) {
}

vk::DeviceQueueCreateInfo queues::bring_info() const {
  vk::DeviceQueueCreateInfo q_description{};
  q_description.sType = vk::StructureType::eDeviceQueueCreateInfo;
  q_description.queueFamilyIndex = m_queues_family_index;
  q_description.queueCount = m_priorities.size();
  q_description.pQueuePriorities = m_priorities.data();
  return q_description;
}

} // namespace pngine
} // namespace csc
