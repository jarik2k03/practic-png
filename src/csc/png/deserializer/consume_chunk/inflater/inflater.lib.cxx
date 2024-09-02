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

  void set_compressed_buffer(const csc::u8unique_buffer& compressed) {
    return this->do_set_compressed_buffer(compressed);
  }
  void inflate(int flush) {
    return this->do_inflate(flush);
  }
  bool done() const {
    return this->do_done();
  }
  auto value() const {
    return static_cast<const_u8unique_buffer_range>(this->do_value());
  }
};

} // namespace csc
