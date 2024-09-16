module;
#include <bits/move.h>
#include <cstdint>
export module csc.pngine.debug_reportEXT;

import :impl;
export namespace csc {
namespace pngine {

class debug_reportEXT : private debug_reportEXT_impl {
 public:
  explicit debug_reportEXT() : debug_reportEXT_impl() {
  }
  explicit debug_reportEXT(vk::Instance& instance) : debug_reportEXT_impl(instance) {
  }
  debug_reportEXT(debug_reportEXT&& move) noexcept : debug_reportEXT_impl(std::move(move)) {
  }
  debug_reportEXT& operator=(debug_reportEXT&& move) noexcept {
    return static_cast<debug_reportEXT&>(debug_reportEXT_impl::operator=(std::move(move)));
  }
  ~debug_reportEXT() = default;
};

} // namespace pngine
} // namespace csc
