module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.semaphore:impl;

export import vulkan_hpp;

namespace csc {
namespace pngine {
class semaphore_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Semaphore m_semaphore{};
  vk::Bool32 m_is_created = false;

 public:
  explicit semaphore_impl() = default;
  explicit semaphore_impl(const vk::Device& device);
  ~semaphore_impl() noexcept;
  semaphore_impl(semaphore_impl&& move) noexcept;
  semaphore_impl& operator=(semaphore_impl&& move) noexcept;
  vk::Semaphore do_get() const;
  void do_clear() noexcept;
};

semaphore_impl::semaphore_impl(semaphore_impl&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_semaphore(move.m_semaphore),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
semaphore_impl& semaphore_impl::operator=(semaphore_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_keep_device = move.m_keep_device;
  m_semaphore = move.m_semaphore;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

semaphore_impl::semaphore_impl(const vk::Device& device) : m_keep_device(&device) {
  vk::SemaphoreCreateInfo description{};
  description.sType = vk::StructureType::eSemaphoreCreateInfo;
  m_semaphore = m_keep_device->createSemaphore(description, nullptr);
  m_is_created = true;
}

void semaphore_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroySemaphore(m_semaphore, nullptr);
    m_is_created = false;
  }
}

vk::Semaphore semaphore_impl::do_get() const {
  return m_semaphore;
}
semaphore_impl::~semaphore_impl() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
