module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance.device.image_view;

import vulkan_hpp;
import :impl;
export namespace csc {
namespace pngine {

class image_view : private image_view_impl {
 public:
  explicit image_view() : image_view_impl() {
  }
  explicit image_view(const vk::Device& device, const vk::Image& image, const vk::Format& img_format)
      : image_view_impl(device, image, img_format) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  auto get() const {
    return this->do_get();
  }
  image_view(image_view&& move) noexcept : image_view_impl(std::move(move)) {
  }
  image_view& operator=(image_view&& move) noexcept {
    return static_cast<image_view&>(image_view_impl::operator=(std::move(move)));
  }
  ~image_view() noexcept = default;
};

} // namespace pngine
} // namespace csc
