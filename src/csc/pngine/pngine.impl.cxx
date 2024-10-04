module;
#include <bits/move.h>
#include <shaders_triangle.h>
module csc.pngine:impl;

export import :attributes;
import stl.string;
export import stl.string_view;
import stl.stdexcept;
#ifndef NDEBUG
import stl.iostream;
#endif
import csc.pngine.instance;
import csc.pngine.instance.device;
import csc.pngine.window_handler;
import vulkan_hpp;

namespace csc {
namespace pngine {

class pngine_impl {
 private:
  std::string m_app_name{"Application"};
  pngine::version m_app_version = pngine::bring_version(1u, 0u, 0u);
  pngine::version m_vk_api_version = pngine::bring_version(1u, 3u, 256u);
  std::string m_gpu_name;
  pngine::instance m_instance;
  pngine::v_window_handler m_window_handler;

 public:
  pngine_impl() = delete;
  pngine_impl(std::string app_name, pngine::version app_version, std::string gpu_name);
  ~pngine_impl() noexcept = default;

  const char* do_get_engine_name() const noexcept;
  pngine::human_version do_get_engine_version() const noexcept;
  pngine::human_version do_get_vk_api_version() const noexcept;
  std::string_view do_get_app_name() const noexcept;
};

pngine_impl::pngine_impl(std::string nm, pngine::version ver, std::string g_nm)
    : m_app_name(std::move(nm)), m_app_version(ver), m_gpu_name(std::move(g_nm)) {
  vk::ApplicationInfo app_info(
      m_app_name.c_str(), m_app_version, pngine::bring_engine_name(), pngine::bring_engine_version(), m_vk_api_version);
  // высокоуровневый алгоритм...
  m_window_handler = pngine::init_window_handler(640u, 480u);
  m_instance = pngine::instance(app_info);
#ifndef NDEBUG
  m_instance.create_debug_reportEXT();
#endif
  m_instance.create_surfaceKHR(m_window_handler);
  m_instance.bring_physical_devices();
  auto& device = m_instance.create_device(m_gpu_name);
  device.create_swapchainKHR();
  device.create_image_views();
  // throw std::runtime_error(CSC_PNGINE_SHADER_PATH_TRIANGLE_VERT);
  device.create_shader_module("vertex", pngine::shaders::shaders_triangle::vert_path);
  device.create_shader_module("fragment", pngine::shaders::shaders_triangle::frag_path);
}

const char* pngine_impl::do_get_engine_name() const noexcept {
  return pngine::bring_engine_name(); // статично, на этапе компиляции
}
pngine::human_version pngine_impl::do_get_engine_version() const noexcept {
  return pngine::bring_human_version(bring_engine_version()); // статично, на этапе компиляции
}
std::string_view pngine_impl::do_get_app_name() const noexcept {
  return m_app_name;
}
pngine::human_version pngine_impl::do_get_vk_api_version() const noexcept {
  return pngine::bring_human_version(m_vk_api_version);
}

} // namespace pngine
} // namespace csc
