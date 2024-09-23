module;
#include <cstdint>
export module csc.pngine;

export import :attributes;
import :impl;

import stl.string_view;
import stl.string;

export namespace csc {
namespace pngine {

class pngine : private pngine_impl {
 public:
  pngine() = delete;

  pngine(std::string app_name, version app_version, std::string gpu_name)
      : pngine_impl(app_name, app_version, gpu_name) {
  }
  ~pngine() noexcept = default;
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
