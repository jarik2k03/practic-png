module;
#include <cstdint>
#include <optional>
module csc.pngine:impl;
export import :attributes;
import stl.stl_wrap.string;
export import stl.stl_wrap.string_view;
#ifndef NDEBUG
import stl.stl_wrap.iostream;
#endif
import csc.pngine.instance;
import vulkan_hpp;

namespace csc {
namespace pngine {

class pngine_impl {
 private:
  std::string m_app_name{"Application"};
  pngine::version m_vk_api_version = pngine::bring_version(1u, 3u, 256u);

  pngine::instance m_instance{};
  vk::ApplicationInfo m_app_info{};

 public:
  pngine_impl() = delete;
  pngine_impl(const char* app_name, version vk_api_version);
  ~pngine_impl() = default;

  void do_init_instance();

  const char* do_get_engine_name() const noexcept;
  pngine::human_version do_get_engine_version() const noexcept;
  pngine::human_version do_get_vk_api_version() const noexcept;
  std::string_view do_get_app_name() const noexcept;
};

pngine_impl::pngine_impl(const char* nm, version vav) : m_app_name(nm), m_vk_api_version(vav) {
  m_app_info = vk::ApplicationInfo(
      m_app_name.c_str(),
      pngine::bring_version(1u, 0u, 0u),
      pngine::bring_engine_name(),
      pngine::bring_engine_version(),
      m_vk_api_version);
}

void pngine_impl::do_init_instance() {
  m_instance = pngine::instance(m_app_info);
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
