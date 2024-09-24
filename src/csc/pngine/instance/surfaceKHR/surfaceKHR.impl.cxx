module;
#include <utility>
module csc.pngine.instance.surfaceKHR:impl;

export import vulkan_hpp;

import stl.stdexcept;
import stl.vector;

import :utility;

namespace csc {
namespace pngine {
class surfaceKHR_impl {
 private:
  vk::SurfaceKHR m_surfaceKHR{};
  vk::Bool32 m_is_created = false;

 public:
  explicit surfaceKHR_impl() = default;
  ~surfaceKHR_impl() noexcept;
  surfaceKHR_impl(surfaceKHR_impl&& move) noexcept = default;
  surfaceKHR_impl& operator=(surfaceKHR_impl&& move) noexcept;
  explicit surfaceKHR_impl(const vk::Instance& instance);
  void do_clear() noexcept;
};

surfaceKHR_impl::~surfaceKHR_impl() noexcept {
  do_clear();
}
surfaceKHR_impl& surfaceKHR_impl::operator=(surfaceKHR_impl&& move) noexcept {
  if (this == &move)
    return *this;
  do_clear();
  m_surfaceKHR = move.m_surfaceKHR;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
surfaceKHR_impl::surfaceKHR_impl(const vk::Instance& instance) {
}

void surfaceKHR_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
