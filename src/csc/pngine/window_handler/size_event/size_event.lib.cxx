export module csc.pngine.window_handler.size_event;

#ifdef _WIN32
export import csc.pngine.window_handler.win32_handler;
#elif __linux
export import csc.pngine.window_handler.xcb_handler;
#endif

export namespace csc {
namespace pngine {

template <typename Pred>
class f_size_event {
 private:
   Pred m_user_code;
 public:
  f_size_event(Pred uc) : m_user_code(uc) {}
#ifdef _WIN32
  auto operator()(pngine::win32_handler& win32) {
    return win32.size_event(m_user_code);
  }
#elif __linux
  auto operator()(pngine::xcb_handler& xcb) {
    return xcb.configure_notify_event(m_user_code);
  }
#endif
};

} // namespace pngine
} // namespace csc


