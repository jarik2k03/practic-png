module;
#include <bits/move.h>
export module csc.pngine.instance;

import stl.string_view;

import csc.pngine.window_handler;

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
  void create_debug_reportEXT() {
    return this->do_create_debug_reportEXT();
  }
  void create_surfaceKHR(const pngine::v_window_handler& handler) {
    return this->do_create_surfaceKHR(handler);
  }
  void create_device(std::string_view gpu_name) {
    return this->do_create_device(gpu_name);
  }
  void bring_physical_devices() {
    return this->do_bring_physical_devices();
  }
  void clear() noexcept {
    return this->do_clear();
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
