module;
#include <cstdint>
export module csc.png.serializer;

export import csc.png.picture;
import cstd.stl_wrap.string_view;
import :impl;
export import csc.png.serializer.produce_chunk.deflater;

export namespace csc {

class serializer : private serializer_impl {
 public:
  serializer() : serializer_impl() {
  }
  ~serializer() = default;

  void serialize(
      cstd::string_view filepath,
      const csc::picture& image,
      csc::e_compression_level level = csc::e_compression_level::default_) {
    return do_serialize(filepath, image, level);
  }
};

} // namespace csc
