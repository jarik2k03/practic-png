module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.fence:impl;

export import vulkan_hpp;

namespace csc {
namespace pngine {
class fence_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Fence m_fence{};
  vk::Bool32 m_is_created = false;

 public:
  explicit fence_impl() = default;
  explicit fence_impl(const vk::Device& device, vk::FenceCreateFlagBits signal);
  ~fence_impl() noexcept;
  fence_impl(fence_impl&& move) noexcept;
  fence_impl& operator=(fence_impl&& move) noexcept;
  vk::Fence do_get() const;
  void do_clear() noexcept;
};

fence_impl::fence_impl(fence_impl&& move) noexcept
    : m_keep_device(move.m_keep_device), m_fence(move.m_fence), m_is_created(std::exchange(move.m_is_created, false)) {
}
fence_impl& fence_impl::operator=(fence_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_keep_device = move.m_keep_device;
  m_fence = move.m_fence;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

fence_impl::fence_impl(const vk::Device& device, vk::FenceCreateFlagBits signal) : m_keep_device(&device) {
  vk::FenceCreateInfo description{};
  description.sType = vk::StructureType::eFenceCreateInfo;
  description.flags = signal;
  m_fence = m_keep_device->createFence(description, nullptr);
  m_is_created = true;
}

void fence_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyFence(m_fence, nullptr);
    m_is_created = false;
  }
}

vk::Fence fence_impl::do_get() const {
  return m_fence;
}
fence_impl::~fence_impl() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
