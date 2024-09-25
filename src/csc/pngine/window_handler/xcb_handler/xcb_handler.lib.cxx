module;
#include <cstdint>
export module csc.pngine.window_handler.xcb_handler;

import :impl;
export namespace csc {
namespace pngine {
class xcb_handler : private xcb_handler_impl {
 public:
  explicit xcb_handler(uint16_t width, uint16_t height) : xcb_handler_impl(width, height) {
  }
  void clear() noexcept {
    this->do_clear();
  }
  xcb_handler() = default;
  xcb_handler(xcb_handler&& move) noexcept = default;
  xcb_handler(const xcb_handler& copy) = delete;
  xcb_handler& operator=(xcb_handler&& move) noexcept = default;
  xcb_handler& operator=(const xcb_handler& copy) = delete;
  ~xcb_handler() noexcept = default;
};

} // namespace pngine
} // namespace csc
