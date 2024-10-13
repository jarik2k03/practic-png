module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.framebuffer:impl;

export import vulkan_hpp;

namespace csc {
namespace pngine {
class framebuffer_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Framebuffer m_framebuffer{};
  vk::Bool32 m_is_created = false;

 public:
  explicit framebuffer_impl() = default;
  explicit framebuffer_impl(
      const vk::Device& device,
      const vk::ImageView& attachment,
      const vk::RenderPass& bind_pass,
      vk::Extent2D swapchain_extent);
  ~framebuffer_impl() noexcept;
  framebuffer_impl(framebuffer_impl&& move) noexcept;
  framebuffer_impl& operator=(framebuffer_impl&& move) noexcept;
  vk::Framebuffer do_get() const;
  void do_clear() noexcept;
};

framebuffer_impl::framebuffer_impl(framebuffer_impl&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_framebuffer(move.m_framebuffer),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
framebuffer_impl& framebuffer_impl::operator=(framebuffer_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_framebuffer = move.m_framebuffer;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

framebuffer_impl::framebuffer_impl(
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

void framebuffer_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyFramebuffer(m_framebuffer, nullptr);
    m_is_created = false;
  }
}

vk::Framebuffer framebuffer_impl::do_get() const {
  return m_framebuffer;
}
framebuffer_impl::~framebuffer_impl() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
