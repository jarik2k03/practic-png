module;
#include <cstdint>
export module csc.png.deserializer;
export import csc.png.png_t;
import csc.stl_wrap.string_view;
import :impl;

export namespace csc {

class deserializer : private deserializer_impl {
 public:
  deserializer() : deserializer_impl() {
  }
  ~deserializer() = default;

  csc::png_t deserialize(csc::string_view filepath) {
    return do_deserialize(filepath);
  }
};

} // namespace csc
