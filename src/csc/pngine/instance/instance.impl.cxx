module;
#include <cstdint>
#include <optional>
#include <memory>
#include <set>
#include <bits/basic_string.h>
module csc.pngine.instance:impl;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.string;
#ifndef NDEBUG
import cstd.stl_wrap.iostream;
#endif
export import vulkan_hpp;

import :utility;
namespace csc {
namespace pngine {
class instance_impl {
 private:
  vk::Instance m_instance{};
  std::set<std::string>* m_vk_extensions{};
  cstd::vector<const char*> m_enabled_extensions{};
  cstd::vector<const char*> m_enabled_layers{};
  vk::InstanceCreateInfo m_description{};

 public:
  explicit instance_impl() = default;
  ~instance_impl() = default;
  instance_impl(instance_impl&& move) noexcept = default;
  instance_impl& operator=(instance_impl&& move) noexcept {
    if (this == &move)
      return *this;
    m_instance = std::move(move.m_instance);
    m_vk_extensions = std::move(move.m_vk_extensions);
    m_enabled_extensions = std::move(move.m_enabled_extensions);
    m_enabled_layers = std::move(move.m_enabled_layers);
    m_description = std::move(move.m_description);
    return *this;
  }
  explicit instance_impl(const vk::ApplicationInfo& app_info);
};

instance_impl::instance_impl(const vk::ApplicationInfo& app_info) {
  m_vk_extensions = const_cast<std::set<std::string>*>(&vk::getInstanceExtensions());
  const char* const surface_ext = pngine::bring_surface_extension();
  [[likely]] if (surface_ext != nullptr) {
    m_enabled_extensions.reserve(256ul);
    m_enabled_extensions.emplace_back("VK_KHR_surface");
    m_enabled_extensions.emplace_back(surface_ext);
  }
#ifndef NDEBUG
  else {
    cstd::cout << "[WARNING]: pngine::instance_impl: " << "Surface extensions are not detected!" << '\n';
  }
#endif
#ifndef NDEBUG
  m_enabled_extensions.emplace_back("VK_DEBUG_report");
#endif
  m_description.sType = vk::StructureType::eInstanceCreateInfo;
  m_description.pNext = nullptr;
  m_description.pApplicationInfo = &app_info;
#ifndef NDEBUG
  m_enabled_layers.reserve(256ul);
  m_enabled_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  m_description.enabledLayerCount = m_enabled_layers.size();
  m_description.ppEnabledLayerNames = m_enabled_layers.data();
#endif
  m_description.enabledExtensionCount = m_enabled_extensions.size();
  m_description.ppEnabledExtensionNames = m_enabled_extensions.data();

  m_instance = vk::createInstance(m_description, nullptr);
}

} // namespace pngine
} // namespace csc
