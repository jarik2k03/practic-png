module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.instance;

import :impl;
export namespace csc {
namespace pngine {

class instance : private instance_impl {
 public:
  explicit instance() : instance_impl() {
  }
  explicit instance(const vk::ApplicationInfo& app_info) : instance_impl(app_info) {
  }
  instance(instance&& move) noexcept = default;
  instance& operator=(instance&& move) noexcept {
    if (this == &move)
      return *this;
    *this = std::move(move);
    return *this;
  }
  ~instance() = default;
};

} // namespace pngine
} // namespace csc
