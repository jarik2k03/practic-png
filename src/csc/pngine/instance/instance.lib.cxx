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
  instance(instance&& move) noexcept : instance_impl(std::move(move)) {
  }
  instance& operator=(instance&& move) noexcept {
    return static_cast<instance&>(instance_impl::operator=(std::move(move)));
  }
  ~instance() = default;
};

} // namespace pngine
} // namespace csc
