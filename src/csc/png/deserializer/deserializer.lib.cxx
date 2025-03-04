module;
#include <cstdint>
export module csc.png.deserializer;

import :impl;
export import csc.png.picture;
import stl.string_view;

export namespace csc {
namespace png {

class deserializer : private deserializer_impl {
 public:
  deserializer() : deserializer_impl() {
  }
  ~deserializer() = default;

  png::picture deserialize(std::string_view filepath, bool ignore_checksum) {
    return do_deserialize(filepath, ignore_checksum);
  }
  void prepare_to_present(png::picture& deserialized) {
    do_prepare_to_present(deserialized);
  }
};

} // namespace png
} // namespace csc
