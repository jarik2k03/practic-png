module;
#include <cstdint>
export module csc.png.serializer;

export import csc.png.picture;
import cstd.stl_wrap.string_view;
import :impl;

export namespace csc {

class serializer : private serializer_impl {
 public:
  serializer() : serializer_impl() {
  }
  ~serializer() = default;

  void serialize(cstd::string_view filepath, const csc::picture& image) {
    return do_serialize(filepath, image);
  }
};

} // namespace csc
