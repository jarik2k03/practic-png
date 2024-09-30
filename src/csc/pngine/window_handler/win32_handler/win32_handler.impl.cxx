module;
#include <cstdint>
#include <windows.h>
module csc.pngine.window_handler.win32_handler:impl;

export namespace csc {
namespace pngine {

class win32_handler_impl {
 private:
  HWND pm_descriptor;
  HISTANCE pm_instance;

 public:
  win32_handler_impl(uint16_t width, uint16_t height) {
  }
};

} // namespace pngine
} // namespace csc
