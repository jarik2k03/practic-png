module;
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.render_pass;

import vulkan_hpp;

export namespace csc {
namespace pngine {
class render_pass {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::RenderPass m_render_pass{};
  vk::Bool32 m_is_created = false;

 public:
  explicit render_pass() = default;
  explicit render_pass(const vk::Device& device, vk::Format swapchain_format);
  ~render_pass() noexcept;
  render_pass(render_pass&& move) noexcept;
  render_pass& operator=(render_pass&& move) noexcept;
  vk::RenderPass get() const;
  void clear() noexcept;
};

render_pass::render_pass(render_pass&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_render_pass(move.m_render_pass),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
render_pass& render_pass::operator=(render_pass&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_render_pass = move.m_render_pass;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

render_pass::render_pass(const vk::Device& device, vk::Format swapchain_format) : m_keep_device(&device) {
  vk::AttachmentDescription color_attachment{};
  color_attachment.format = swapchain_format;
  color_attachment.samples = vk::SampleCountFlagBits::e1;

  color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
  color_attachment.storeOp = vk::AttachmentStoreOp::eStore;

  color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

  color_attachment.initialLayout = vk::ImageLayout::eUndefined;
  color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0u;
  color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::SubpassDescription subpass{};
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.colorAttachmentCount = 1u;
  // рендер-пасс с цветовым назначением буфера и подпроходом с индексом изображения - 0
  vk::RenderPassCreateInfo description{};
  description.sType = vk::StructureType::eRenderPassCreateInfo;
  description.pAttachments = &color_attachment;
  description.attachmentCount = 1u;

  description.pSubpasses = &subpass;
  description.subpassCount = 1u;

  m_render_pass = m_keep_device->createRenderPass(description, nullptr);
  m_is_created = true;
}

void render_pass::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyRenderPass(m_render_pass, nullptr);
    m_is_created = false;
  }
}

vk::RenderPass render_pass::get() const {
  return m_render_pass;
}
render_pass::~render_pass() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
