module;
#include <cstdint>
export module csc.png.png_t.sections.inflater;

import :impl;
export import :attributes;

export namespace csc {

class inflater : private inflater_impl {
 public:
  inflater() : inflater_impl() {
  }

  ~inflater() noexcept = default;

  void set_compressed_buffer(const csc::vector<uint8_t>& compressed) {
    return this->do_set_compressed_buffer(compressed);
  }
  void inflate() {
    return this->do_inflate();
  }
  bool done() const {
    return this->do_done();
  }
  uncompressed_range value() const {
    return static_cast<uncompressed_range>(this->do_value());
  }
};

} // namespace csc
