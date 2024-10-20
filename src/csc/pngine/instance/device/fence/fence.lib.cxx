module;
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.fence;

import vulkan_hpp;

export namespace csc {
namespace pngine {
class fence {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Fence m_fence{};
  vk::Bool32 m_is_created = false;

 public:
  explicit fence() = default;
  explicit fence(const vk::Device& device, vk::FenceCreateFlagBits signal);
  ~fence() noexcept;
  fence(fence&& move) noexcept;
  fence& operator=(fence&& move) noexcept;
  vk::Fence get() const;
  void clear() noexcept;
};

fence::fence(fence&& move) noexcept
    : m_keep_device(move.m_keep_device), m_fence(move.m_fence), m_is_created(std::exchange(move.m_is_created, false)) {
}
fence& fence::operator=(fence&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_keep_device = move.m_keep_device;
  m_fence = move.m_fence;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

fence::fence(const vk::Device& device, vk::FenceCreateFlagBits signal) : m_keep_device(&device) {
  vk::FenceCreateInfo description{};
  description.sType = vk::StructureType::eFenceCreateInfo;
  description.flags = signal;
  m_fence = m_keep_device->createFence(description, nullptr);
  m_is_created = true;
}

void fence::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyFence(m_fence, nullptr);
    m_is_created = false;
  }
}

vk::Fence fence::get() const {
  return m_fence;
}
fence::~fence() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
