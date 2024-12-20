module;
#include <utility>
#include <vulkan/vulkan_core.h>
export module csc.pngine.instance.surfaceKHR;

import vulkan_hpp;

export namespace csc {
namespace pngine {
class surfaceKHR {
 private:
  vk::SurfaceKHR m_surfaceKHR{};
  const vk::Instance* m_keep_instance = nullptr;
  vk::Bool32 m_is_created = false;

 public:
  explicit surfaceKHR() = default;
  ~surfaceKHR() noexcept;
  surfaceKHR(surfaceKHR&& move) noexcept;
  surfaceKHR& operator=(surfaceKHR&& move) noexcept;
  explicit surfaceKHR(const vk::Instance& instance, const vk::SurfaceKHR& surface);
  const vk::SurfaceKHR& get() const noexcept;
  void clear() noexcept;
};

surfaceKHR::~surfaceKHR() noexcept {
  clear();
}

surfaceKHR::surfaceKHR(surfaceKHR&& move) noexcept
    : m_surfaceKHR(move.m_surfaceKHR),
      m_keep_instance(move.m_keep_instance),
      m_is_created(std::exchange(move.m_is_created, false)) {
}

surfaceKHR& surfaceKHR::operator=(surfaceKHR&& move) noexcept {
  if (this == &move)
    return *this;
  clear();
  m_surfaceKHR = move.m_surfaceKHR;
  m_keep_instance = move.m_keep_instance;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
surfaceKHR::surfaceKHR(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  m_surfaceKHR = surface;
  m_keep_instance = &instance;
  m_is_created = true;
}

const vk::SurfaceKHR& surfaceKHR::get() const noexcept {
  return m_surfaceKHR;
}

void surfaceKHR::clear() noexcept {
  if (m_is_created != false) {
    struct surface_dispatch : vk::detail::DispatchLoaderBase {
      PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
    };
    surface_dispatch dispatch{};
    dispatch.vkDestroySurfaceKHR =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(m_keep_instance->getProcAddr("vkDestroySurfaceKHR"));
    m_keep_instance->destroySurfaceKHR(m_surfaceKHR, nullptr, dispatch);
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
