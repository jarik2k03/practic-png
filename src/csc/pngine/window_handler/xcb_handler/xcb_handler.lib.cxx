module;
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <xcb/xcb.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>
export module csc.pngine.window_handler.xcb_handler;

import vulkan_hpp;
import stl.stdexcept;
import stl.string_view;

namespace csc {
namespace pngine {

constexpr const char* generate_error_message_for_connection(int xcb_connect_status) {
  switch (xcb_connect_status) {
    case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
      return "xcb::Отсутствуют расширения для подключения";
    case XCB_CONN_CLOSED_PARSE_ERR:
      return "xcb::Не удалось пропарсить название экрана. (Вероятно, отсутствует LF или CRLF.)";
    case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
      return "xcb::Недостаточно памяти для подключения.";
    case XCB_CONN_CLOSED_INVALID_SCREEN:
      return "xcb::Не найден подходящий экран для последующего отображения";
    case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
      return "xcb::Превышено время запроса, который сервер мог бы принять";
    case XCB_CONN_ERROR:
      return "xcb::Ошибка подключения к xcb-серверу!";
  }
  return "xcb::Неизвестная ошибка подключения к XCB";
}

} // namespace pngine
} // namespace csc

export namespace csc {
namespace pngine {

class xcb_handler {
 private:
  xcb_connection_t* mp_xcb_connect = nullptr;
  xcb_screen_t* mp_xcb_selected_screen = nullptr;
  xcb_window_t m_xcb_window = 0;
  xcb_generic_event_t* mp_cur_event = nullptr;
  std::string_view m_window_name{};

 public:
  xcb_handler(uint16_t width, uint16_t height, std::string_view window_name);

  xcb_handler() = default;
  xcb_handler(xcb_handler&& move) noexcept;
  xcb_handler& operator=(xcb_handler&& move) noexcept;
  ~xcb_handler() noexcept;

  void clear() noexcept;
  vk::SurfaceKHR create_surface(const vk::Instance& instance) const;
  template <typename Pred>
  void expose_event(Pred user_code) const;
  bool wait_events();
};
xcb_handler::xcb_handler(uint16_t window_width, uint16_t window_height, std::string_view window_name)
    : m_window_name(window_name) {
  mp_xcb_connect = ::xcb_connect(nullptr, nullptr);
  auto err_code = ::xcb_connection_has_error(mp_xcb_connect);
  if (err_code != 0) {
    mp_xcb_connect = nullptr; // на всякий случай чтобы clear в dtor не сломал прогу
    throw std::runtime_error(pngine::generate_error_message_for_connection(err_code));
  }
  const xcb_setup_t* setup = ::xcb_get_setup(mp_xcb_connect);
  xcb_screen_iterator_t screen_pos = ::xcb_setup_roots_iterator(setup);
  mp_xcb_selected_screen = screen_pos.data;
  m_xcb_window = ::xcb_generate_id(mp_xcb_connect);
  uint32_t required_events[] = {XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_EXPOSURE};
  ::xcb_create_window(
      mp_xcb_connect,
      XCB_COPY_FROM_PARENT,
      m_xcb_window,
      mp_xcb_selected_screen->root,
      0,
      0,
      window_width,
      window_height,
      16,
      XCB_WINDOW_CLASS_INPUT_OUTPUT,
      mp_xcb_selected_screen->root_visual,
      XCB_CW_EVENT_MASK,
      required_events);

  ::xcb_change_property(
      mp_xcb_connect,
      XCB_PROP_MODE_APPEND,
      m_xcb_window,
      XCB_ATOM_WM_NAME,
      XCB_ATOM_STRING,
      8,
      m_window_name.size(),
      m_window_name.data());

  ::xcb_map_window(mp_xcb_connect, m_xcb_window);
  ::xcb_flush(mp_xcb_connect);
}
xcb_handler::~xcb_handler() noexcept {
  clear();
}
void xcb_handler::clear() noexcept {
  if (mp_xcb_connect != nullptr) {
    ::xcb_destroy_window(mp_xcb_connect, m_xcb_window);
    ::xcb_disconnect(mp_xcb_connect), mp_xcb_connect = nullptr;
  }
  if (mp_cur_event != nullptr)
    ::free(mp_cur_event), mp_cur_event = nullptr;
}

xcb_handler& xcb_handler::operator=(xcb_handler&& move) noexcept {
  [[unlikely]] if (this == &move)
    return *this;
  clear();
  m_xcb_window = std::exchange(move.m_xcb_window, 0);
  mp_xcb_connect = std::exchange(move.mp_xcb_connect, nullptr);
  mp_xcb_selected_screen = std::exchange(move.mp_xcb_selected_screen, nullptr);
  mp_cur_event = std::exchange(move.mp_cur_event, nullptr);
  m_window_name = move.m_window_name;
  return *this;
}

xcb_handler::xcb_handler(xcb_handler&& move) noexcept
    : mp_xcb_connect(std::exchange(move.mp_xcb_connect, nullptr)),
      mp_xcb_selected_screen(std::exchange(move.mp_xcb_selected_screen, nullptr)),
      m_xcb_window(std::exchange(move.m_xcb_window, 0)),
      mp_cur_event(std::exchange(move.mp_cur_event, nullptr)),
      m_window_name(move.m_window_name) {
}

vk::SurfaceKHR xcb_handler::create_surface(const vk::Instance& instance) const {
  if (mp_xcb_connect == nullptr)
    throw std::runtime_error("Window is not created. Surface depends to window.");
  VkXcbSurfaceCreateInfoKHR xcb_surface_desc{};
  xcb_surface_desc.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  xcb_surface_desc.pNext = nullptr;
  xcb_surface_desc.connection = mp_xcb_connect;
  xcb_surface_desc.window = m_xcb_window;
  xcb_surface_desc.flags = 0;

  auto pfn_vkCreateXcbSurfaceKHR = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(instance.getProcAddr("vkCreateXcbSurfaceKHR"));
  if (pfn_vkCreateXcbSurfaceKHR == nullptr)
    throw std::runtime_error("vkCreateXcbSurfaceKHR call is not found!");
  VkSurfaceKHR vksurface;
  static_assert(std::is_pointer_v<VkInstance>);
  VkResult vr =
      pfn_vkCreateXcbSurfaceKHR(static_cast<const VkInstance>(instance), &xcb_surface_desc, nullptr, &vksurface);
  if (vr != VK_SUCCESS)
    throw std::runtime_error("Failed to create window surface!");
  return vk::SurfaceKHR(vksurface);
}

bool xcb_handler::wait_events() {
  if (mp_xcb_connect == nullptr)
    return false;
  if (mp_cur_event != nullptr)
    ::free(mp_cur_event);
  mp_cur_event = ::xcb_wait_for_event(mp_xcb_connect);
  return mp_cur_event != nullptr;
}

template <typename Pred>
void xcb_handler::expose_event(Pred user_code) const {
  if ((mp_cur_event->response_type & 0x7f) == XCB_EXPOSE)
    user_code(); // колбэк определяется на этапе компиляции
}
// auto xcb_handler::

} // namespace pngine
} // namespace csc
