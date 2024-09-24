module;
#include <cstdint>
export module csc.pngine.window_handler.win32_handler;

import :impl;
export namespace csc {
namespace pngine {
class win32_handler : private win32_handler_impl {
 public:
  explicit win32_handler(uint32_t width, uint32_t height) : win32_handler_impl(width, height) {
  }
  win32_handler(win32_handler&& move) noexcept = delete;
  win32_handler(const win32_handler& copy) = delete;
  win32_handler& operator=(win32_handler&& move) noexcept = delete;
  win32_handler& operator=(const win32_handler& copy) = delete;
  ~win32_handler() noexcept = default;
};

} // namespace pngine
} // namespace csc
