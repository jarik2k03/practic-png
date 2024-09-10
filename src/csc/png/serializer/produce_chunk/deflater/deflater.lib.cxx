
module;
#include <bits/move.h>
#include <cstdint>

export module csc.png.serializer.produce_chunk.deflater;
export import csc.png.serializer.produce_chunk.deflater.attributes;

import :impl;

export namespace csc {

class deflater : private deflater_impl {
 public:
  explicit deflater() : deflater_impl() {
  }
  explicit deflater(
      csc::e_compression_level level,
      csc::e_compression_strategy strategy = e_compression_strategy::default_,
      int32_t mem_level = 8,
      int32_t win_bits = 15)
      : deflater_impl(level, strategy, mem_level, win_bits) {
  }
  ~deflater() noexcept = default;

  deflater(const csc::deflater& copy) = delete;
  auto& operator=(const csc::deflater& copy) = delete;

  deflater(csc::deflater&& move) noexcept = default; // использует ctor от impl
  csc::deflater& operator=(csc::deflater&& move) noexcept = default; // исп. присваивание от impl

  void flush(csc::u8buffer_view decompressed) {
    return this->do_flush(decompressed);
  }
  void deflate(uint32_t stride_read) {
    return this->do_deflate(stride_read);
  }
  bool done() const {
    return this->do_done();
  }
  auto value() const {
    return static_cast<const_u8buffer_range>(this->do_value());
  }
};

} // namespace csc
