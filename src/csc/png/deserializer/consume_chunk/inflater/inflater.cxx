module;
#include <bits/move.h>
#include <cstdint>

export module csc.png.deserializer.consume_chunk.inflater;

import :impl;
export import :attributes;

export namespace csc {

class inflater : private inflater_impl {
 public:
  explicit inflater() : inflater_impl() {
  }

  ~inflater() noexcept = default;

  inflater(const csc::inflater& copy) = delete;
  auto& operator=(const csc::inflater& copy) = delete;

  inflater(csc::inflater&& move) noexcept = default; // использует ctor от impl
  csc::inflater& operator=(csc::inflater&& move) noexcept = default; // исп. присваивание от impl

  void set_compressed_buffer(const cstd::vector<uint8_t>& compressed) {
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
