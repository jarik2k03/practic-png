module;
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.framebuffer;

import vulkan_hpp;

export namespace csc {
namespace pngine {
class framebuffer {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Framebuffer m_framebuffer{};
  vk::Bool32 m_is_created = false;

 public:
  explicit framebuffer() = default;
  explicit framebuffer(
      const vk::Device& device,
      const vk::ImageView& attachment,
      const vk::RenderPass& bind_pass,
      vk::Extent2D swapchain_extent);
  ~framebuffer() noexcept;
  framebuffer(framebuffer&& move) noexcept;
  framebuffer& operator=(framebuffer&& move) noexcept;
  vk::Framebuffer get() const;
  void clear() noexcept;
};

framebuffer::framebuffer(framebuffer&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_framebuffer(move.m_framebuffer),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
framebuffer& framebuffer::operator=(framebuffer&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_framebuffer = move.m_framebuffer;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

framebuffer::framebuffer(
    const vk::Device& device,
    const vk::ImageView& attachment,
    const vk::RenderPass& bind_pass,
    vk::Extent2D swapchain_extent)
    : m_keep_device(&device) {
  vk::FramebufferCreateInfo description{};
  description.sType = vk::StructureType::eFramebufferCreateInfo;
  description.renderPass = bind_pass;
  description.pAttachments = &attachment;
  description.attachmentCount = 1u;

  description.width = swapchain_extent.width;
  description.height = swapchain_extent.height;
  description.layers = 1u;

  m_framebuffer = m_keep_device->createFramebuffer(description, nullptr);
  m_is_created = true;
}

void framebuffer::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyFramebuffer(m_framebuffer, nullptr);
    m_is_created = false;
  }
}

vk::Framebuffer framebuffer::get() const {
  return m_framebuffer;
}
framebuffer::~framebuffer() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
