
module;
#include <bits/move.h>
#include <bits/allocator.h>
#include <cstdint>

export module csc.png.serializer.produce_chunk.deflater;
export import csc.png.serializer.produce_chunk.deflater.attributes;

import :impl;

export namespace csc {
namespace png {

template <typename Alloc>
class deflater : private deflater_impl<Alloc> {
 public:
  explicit deflater() : deflater_impl<Alloc>() {
  }
  explicit deflater(const Alloc& allocator) : deflater_impl<Alloc>(allocator) {
  }
  explicit deflater(
      png::e_compression_level level,
      png::e_compression_strategy strategy = e_compression_strategy::default_,
      int32_t mem_level = 8,
      int32_t win_bits = 15)
      : deflater_impl<Alloc>(level, strategy, mem_level, win_bits) {
  }
  ~deflater() noexcept = default;

  deflater(const png::deflater<Alloc>& copy) = delete;
  auto& operator=(const png::deflater<Alloc>& copy) = delete;

  deflater(png::deflater<Alloc>&& move) noexcept = default; // использует ctor от impl
  png::deflater<Alloc>& operator=(png::deflater<Alloc>&& move) noexcept = default; // исп. присваивание от impl

  void flush(png::u8buffer_view decompressed) {
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

using common_deflater = png::deflater<std::allocator<uint8_t>>;

} // namespace png
} // namespace csc
