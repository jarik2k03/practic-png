module;
#include <cstdint>
export module csc.png.serializer_lib;

export import csc.png.png_t;
import csc.stl_wrap.string_view;
import :impl;

export namespace csc {

class serializer : private serializer_impl {
 public:
  serializer() : serializer_impl() {
  }
  ~serializer() = default;

  void serialize(csc::string_view filepath, const csc::png_t& image) {
    return do_serialize(filepath, image);
  }
};

} // namespace csc
