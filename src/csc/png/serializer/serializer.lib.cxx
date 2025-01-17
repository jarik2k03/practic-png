module;
#include <cstdint>
export module csc.png.serializer;

export import csc.png.picture;
import stl.string_view;
import :impl;
export import csc.png.serializer.produce_chunk.deflater.attributes;

export namespace csc {
namespace png {

class serializer : private serializer_impl {
 public:
  serializer() : serializer_impl() {
  }
  ~serializer() = default;

  void serialize(
      std::string_view filepath,
      const png::picture& image,
      e_compression_level level = png::e_compression_level::default_,
      int32_t memory_usage = 8,
      int32_t win_bits = 15,
      e_compression_strategy strategy = png::e_compression_strategy::default_) {
    return do_serialize(filepath, image, level, memory_usage, win_bits, strategy);
  }
};

} // namespace png
} // namespace csc
