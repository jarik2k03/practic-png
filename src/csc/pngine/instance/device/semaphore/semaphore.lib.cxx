module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.semaphore;

import vulkan_hpp;

export namespace csc {
namespace pngine {
class semaphore {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Semaphore m_semaphore{};
  vk::Bool32 m_is_created = false;

 public:
  explicit semaphore() = default;
  explicit semaphore(const vk::Device& device);
  ~semaphore() noexcept;
  semaphore(semaphore&& move) noexcept;
  semaphore& operator=(semaphore&& move) noexcept;
  vk::Semaphore do_get() const;
  void do_clear() noexcept;
};

semaphore::semaphore(semaphore&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_semaphore(move.m_semaphore),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
semaphore& semaphore::operator=(semaphore&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_keep_device = move.m_keep_device;
  m_semaphore = move.m_semaphore;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

semaphore::semaphore(const vk::Device& device) : m_keep_device(&device) {
  vk::SemaphoreCreateInfo description{};
  description.sType = vk::StructureType::eSemaphoreCreateInfo;
  m_semaphore = m_keep_device->createSemaphore(description, nullptr);
  m_is_created = true;
}

void semaphore::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroySemaphore(m_semaphore, nullptr);
    m_is_created = false;
  }
}

vk::Semaphore semaphore::do_get() const {
  return m_semaphore;
}
semaphore::~semaphore() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
