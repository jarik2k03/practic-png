module;
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.command_pool;

import vulkan_hpp;

import stl.vector;

export namespace csc {
namespace pngine {
class command_pool {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::CommandPool m_command_pool{};
  vk::Bool32 m_is_created = false;

 public:
  explicit command_pool() = default;
  explicit command_pool(const vk::Device& device, uint32_t queue_family);
  ~command_pool() noexcept;
  command_pool(command_pool&& move) noexcept;
  command_pool& operator=(command_pool&& move) noexcept;
  vk::CommandPool get() const;
  std::vector<vk::CommandBuffer> allocate_command_buffers(vk::CommandBufferLevel level, uint32_t count) const;
  void clear() noexcept;
};

command_pool::command_pool(command_pool&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_command_pool(move.m_command_pool),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
command_pool& command_pool::operator=(command_pool&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_command_pool = move.m_command_pool;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

command_pool::command_pool(const vk::Device& device, uint32_t family_index) : m_keep_device(&device) {
  vk::CommandPoolCreateInfo description{};
  description.sType = vk::StructureType::eCommandPoolCreateInfo;
  description.queueFamilyIndex = family_index;
  description.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  m_command_pool = m_keep_device->createCommandPool(description, nullptr);
  m_is_created = true;
}

void command_pool::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyCommandPool(m_command_pool, nullptr);
    m_is_created = false;
  }
}

std::vector<vk::CommandBuffer> command_pool::allocate_command_buffers(vk::CommandBufferLevel level, uint32_t count)
    const {
  vk::CommandBufferAllocateInfo alloc_desc{};
  alloc_desc.sType = vk::StructureType::eCommandBufferAllocateInfo;
  alloc_desc.commandPool = m_command_pool;
  alloc_desc.level = level;
  alloc_desc.commandBufferCount = count;

  return m_keep_device->allocateCommandBuffers(alloc_desc);
}

vk::CommandPool command_pool::get() const {
  return m_command_pool;
}
command_pool::~command_pool() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
