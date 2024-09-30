module;
#include <utility>
module csc.pngine.instance.surfaceKHR:impl;

export import vulkan_hpp;

import :utility;

namespace csc {
namespace pngine {
class surfaceKHR_impl {
 private:
  vk::SurfaceKHR m_surfaceKHR{};
  const vk::Instance* m_keep_instance = nullptr;
  vk::Bool32 m_is_created = false;

 public:
  explicit surfaceKHR_impl() = default;
  ~surfaceKHR_impl() noexcept;
  surfaceKHR_impl(surfaceKHR_impl&& move) noexcept;
  surfaceKHR_impl& operator=(surfaceKHR_impl&& move) noexcept;
  explicit surfaceKHR_impl(const vk::Instance& instance, const vk::SurfaceKHR& surface);
  const vk::SurfaceKHR& do_get() const noexcept;
  void do_clear() noexcept;
};

surfaceKHR_impl::~surfaceKHR_impl() noexcept {
  do_clear();
}

surfaceKHR_impl::surfaceKHR_impl(surfaceKHR_impl&& move) noexcept
    : m_surfaceKHR(move.m_surfaceKHR),
      m_keep_instance(move.m_keep_instance),
      m_is_created(std::exchange(move.m_is_created, false)) {
}

surfaceKHR_impl& surfaceKHR_impl::operator=(surfaceKHR_impl&& move) noexcept {
  if (this == &move)
    return *this;
  do_clear();
  m_surfaceKHR = move.m_surfaceKHR;
  m_keep_instance = move.m_keep_instance;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
surfaceKHR_impl::surfaceKHR_impl(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  m_surfaceKHR = surface;
  m_keep_instance = &instance;
  m_is_created = true;
}

const vk::SurfaceKHR& surfaceKHR_impl::do_get() const noexcept {
  return m_surfaceKHR;
}

void surfaceKHR_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_instance->destroySurfaceKHR(m_surfaceKHR, {}, {});
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
