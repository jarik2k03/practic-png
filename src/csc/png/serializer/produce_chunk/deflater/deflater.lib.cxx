
module;
#include <bits/move.h>
#include <cstdint>

export module csc.png.serializer.produce_chunk.deflater;

import :impl;
export import :attributes;

export namespace csc {

class deflater : private deflater_impl {
 public:
  explicit deflater() : deflater_impl() {
  }

  ~deflater() noexcept = default;

  deflater(const csc::deflater& copy) = delete;
  auto& operator=(const csc::deflater& copy) = delete;

  deflater(csc::deflater&& move) noexcept = default; // использует ctor от impl
  csc::deflater& operator=(csc::deflater&& move) noexcept = default; // исп. присваивание от impl

  void set_decompressed_buffer(const cstd::vector<uint8_t>& decompressed) {
    return this->do_set_decompressed_buffer(decompressed);
  }
  void deflate() {
    return this->do_deflate();
  }
  bool done() const {
    return this->do_done();
  }
  auto value() const {
    return static_cast<const_u8unique_buffer_range>(this->do_value());
  }
};

} // namespace csc
