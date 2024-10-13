module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.image_view:impl;

export import vulkan_hpp;

namespace csc {
namespace pngine {
class image_view_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::ImageView m_image_view{};
  vk::Bool32 m_is_created = false;

 public:
  explicit image_view_impl() = default;
  explicit image_view_impl(const vk::Device& device, const vk::Image& image, const vk::Format& img_format);
  ~image_view_impl() noexcept;
  image_view_impl(image_view_impl&& move) noexcept;
  image_view_impl& operator=(image_view_impl&& move) noexcept;
  void do_clear() noexcept;
  vk::ImageView do_get() const;
};

image_view_impl::image_view_impl(image_view_impl&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_image_view(move.m_image_view),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
image_view_impl& image_view_impl::operator=(image_view_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_image_view = move.m_image_view;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

image_view_impl::image_view_impl(const vk::Device& device, const vk::Image& image, const vk::Format& img_format)
    : m_keep_device(&device) {
  vk::ImageViewCreateInfo description{};
  description.sType = vk::StructureType::eImageViewCreateInfo;
  description.pNext = nullptr;
  description.image = image;
  description.viewType = vk::ImageViewType::e2D;
  description.format = img_format;
  description.components.r = vk::ComponentSwizzle::eIdentity;
  description.components.g = vk::ComponentSwizzle::eIdentity;
  description.components.b = vk::ComponentSwizzle::eIdentity;
  description.components.a = vk::ComponentSwizzle::eIdentity;
  description.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  description.subresourceRange.baseMipLevel = 0;
  description.subresourceRange.levelCount = 1;
  description.subresourceRange.baseArrayLayer = 0;
  description.subresourceRange.layerCount = 1;

  m_image_view = m_keep_device->createImageView(description, nullptr);
  m_is_created = true;
}

void image_view_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyImageView(m_image_view, nullptr);
    m_is_created = false;
  }
}

vk::ImageView image_view_impl::do_get() const {
  return m_image_view;
}

image_view_impl::~image_view_impl() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
