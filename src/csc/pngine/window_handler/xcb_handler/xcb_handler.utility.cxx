module;
#include <xcb/xcb.h>
module csc.pngine.window_handler.xcb_handler:utility;

export namespace csc {
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
