module;

export module csc.pngine.window_handler;

export import :visitors;

import stl.variant;

export namespace csc {
namespace pngine {

#ifdef _WIN32
using v_window_handler = std::variant<pngine::win32_handler>;
#elif __linux
using v_window_handler = std::variant<pngine::xcb_handler>;
#endif
// вариант - потому что в линуксе использование Вяленого или Иксов не определено на этапе компиляции.
} // namespace pngine
} // namespace csc
