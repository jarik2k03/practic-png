module;
#include <cstdint>
export module csc.pngine;

export import :attributes;
import :impl;

import stl.string_view;

export namespace csc {
namespace pngine {

class pngine : private pngine_impl {
 public:
  pngine() = delete;

  pngine(const char* app_name, version vk_api_version) : pngine_impl(app_name, vk_api_version) {
  }
  ~pngine() = default;
  void init_instance() {
    this->do_init_instance();
  }
  std::string_view get_app_name() const noexcept {
    return this->do_get_app_name();
  }
  const char* get_engine_name() const noexcept {
    return this->do_get_engine_name();
  }
  human_version get_engine_version() const noexcept {
    return this->do_get_engine_version();
  }
  human_version get_vk_api_version() const noexcept {
    return this->do_get_vk_api_version();
  }
};

} // namespace pngine
} // namespace csc
