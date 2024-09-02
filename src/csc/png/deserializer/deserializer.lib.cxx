module;
#include <cstdint>
export module csc.png.deserializer;

import :impl;
export import csc.png.picture;
import cstd.stl_wrap.string_view;

export namespace csc {

class deserializer : private deserializer_impl {
 public:
  deserializer() : deserializer_impl() {
  }
  ~deserializer() = default;

  csc::picture deserialize(cstd::string_view filepath) {
    return do_deserialize(filepath);
  }
};

} // namespace csc
