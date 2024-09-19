module;
#include <bits/move.h>
export module csc.pngine.instance;

import vulkan_hpp;
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
#ifndef NDEBUG
  void create_debug_reportEXT() {
    return this->do_create_debug_reportEXT();
  }
#endif
  void create_device() {
    return this->do_create_device();
  }
  void bring_physical_devices() {
    return this->do_bring_physical_devices();
  }
  vk::Instance& get() {
    return this->do_get();
  }
  const vk::Instance& get() const {
    return this->do_get();
  }
  ~instance() noexcept = default;
};

} // namespace pngine
} // namespace csc
