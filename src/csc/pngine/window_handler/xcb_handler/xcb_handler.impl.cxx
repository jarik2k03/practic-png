module;
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <xcb/xcb.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xcb.h>
module csc.pngine.window_handler.xcb_handler:impl;

import vulkan_hpp;
import stl.stdexcept;

import :utility;
export namespace csc {
namespace pngine {

class xcb_handler_impl {
 private:
  xcb_connection_t* mp_xcb_connect = nullptr;
  xcb_screen_t* mp_xcb_selected_screen = nullptr;
  xcb_window_t m_xcb_window = 0;
  xcb_generic_event_t* mp_cur_event = nullptr;

 public:
  xcb_handler_impl(uint16_t width, uint16_t height);

  xcb_handler_impl() = default;
  xcb_handler_impl(xcb_handler_impl&& move) noexcept;
  xcb_handler_impl& operator=(xcb_handler_impl&& move) noexcept;
  ~xcb_handler_impl() noexcept;

  void do_clear() noexcept;
  vk::SurfaceKHR do_create_surface(const vk::Instance& instance) const;
  bool do_poll_event();
};
xcb_handler_impl::xcb_handler_impl(uint16_t window_width, uint16_t window_height) {
  mp_xcb_connect = ::xcb_connect(nullptr, nullptr);
  auto err_code = ::xcb_connection_has_error(mp_xcb_connect);
  if (err_code != 0) {
    mp_xcb_connect = nullptr; // на всякий случай чтобы do_clear в dtor не сломал прогу
    throw std::runtime_error(pngine::generate_error_message_for_connection(err_code));
  }
  const xcb_setup_t* setup = ::xcb_get_setup(mp_xcb_connect);
  xcb_screen_iterator_t screen_pos = ::xcb_setup_roots_iterator(setup);
  mp_xcb_selected_screen = screen_pos.data;
  m_xcb_window = ::xcb_generate_id(mp_xcb_connect);
  uint32_t required_events[] = { XCB_EVENT_MASK_STRUCTURE_NOTIFY };
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
  ::xcb_map_window(mp_xcb_connect, m_xcb_window);
  ::xcb_flush(mp_xcb_connect);
}
xcb_handler_impl::~xcb_handler_impl() noexcept {
  do_clear();
}
void xcb_handler_impl::do_clear() noexcept {
  if (mp_xcb_connect != nullptr)
    ::xcb_destroy_window(mp_xcb_connect, m_xcb_window);
  if (mp_xcb_connect != nullptr)
    ::xcb_disconnect(mp_xcb_connect), mp_xcb_connect = nullptr;
  if (mp_cur_event != nullptr)
    ::free(mp_cur_event);
}

xcb_handler_impl& xcb_handler_impl::operator=(xcb_handler_impl&& move) noexcept {
  [[unlikely]] if (this == &move)
    return *this;
  do_clear();
  m_xcb_window = std::exchange(move.m_xcb_window, 0);
  mp_xcb_connect = std::exchange(move.mp_xcb_connect, nullptr);
  mp_xcb_selected_screen = std::exchange(move.mp_xcb_selected_screen, nullptr);
  mp_cur_event = std::exchange(move.mp_cur_event, nullptr);
  return *this;
}

xcb_handler_impl::xcb_handler_impl(xcb_handler_impl&& move) noexcept
    : mp_xcb_connect(std::exchange(move.mp_xcb_connect, nullptr)),
      mp_xcb_selected_screen(std::exchange(move.mp_xcb_selected_screen, nullptr)),
      m_xcb_window(std::exchange(move.m_xcb_window, 0)),
      mp_cur_event(std::exchange(move.mp_cur_event, nullptr))
{
}

vk::SurfaceKHR xcb_handler_impl::do_create_surface(const vk::Instance& instance) const {
  VkXcbSurfaceCreateInfoKHR xcb_surface_desc;
  xcb_surface_desc.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  xcb_surface_desc.pNext = nullptr;
  xcb_surface_desc.connection = mp_xcb_connect;
  xcb_surface_desc.window = m_xcb_window;
  xcb_surface_desc.flags = 0;

  VkSurfaceKHR vksurface;
  static_assert(std::is_pointer_v<VkInstance>);
  VkResult vr =
      ::vkCreateXcbSurfaceKHR(static_cast<const VkInstance>(instance), &xcb_surface_desc, nullptr, &vksurface);
  if (vr != VK_SUCCESS)
    throw std::runtime_error("Failed to create window!");
  return vk::SurfaceKHR(vksurface);
}

bool xcb_handler_impl::do_poll_event() {
  if (mp_xcb_connect == nullptr)
    return false;
  if (mp_cur_event != nullptr)
    ::free(mp_cur_event);
  mp_cur_event = ::xcb_wait_for_event(mp_xcb_connect);
  return mp_cur_event != nullptr;
}


// auto xcb_handler_impl::

} // namespace pngine
} // namespace csc
