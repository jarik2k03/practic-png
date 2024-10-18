export module csc.pngine.window_handler.poll_event;

#ifdef _WIN32
export import csc.pngine.window_handler.win32_handler;
#elif __linux
export import csc.pngine.window_handler.xcb_handler;
#endif

export namespace csc {
namespace pngine {

class f_poll_event {
 public:
  f_poll_event() = default;
#ifdef _WIN32
  auto operator()(pngine::win32_handler& win32) {
    return win32.poll_event();
  }
#elif __linux
  auto operator()(pngine::xcb_handler& xcb) {
    return xcb.poll_event();
  }
#endif
};

} // namespace pngine
} // namespace csc

