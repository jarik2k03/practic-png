module;
#include <cstdint>
#include <xcb/xcb.h>
module csc.pngine.window_handler.xcb_handler:impl;

export namespace csc {
namespace pngine {

class xcb_handler_impl {
 private:
  xcb_connection_t* mp_xcb_connect;
  xcb_screen_t* mp_xcb_screen;
  xcb_window_t m_xcb_window;

 public:
  xcb_handler_impl(uint32_t width, uint32_t height) {
  }
};

} // namespace pngine
} // namespace csc
