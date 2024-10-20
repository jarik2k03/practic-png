module;
#include <bits/move.h>
#include <bits/allocator.h>
#include <cstdint>

export module csc.png.deserializer.consume_chunk.inflater;

import :impl;

export namespace csc {
namespace png {

template <typename Alloc>
class inflater : private inflater_impl<Alloc> {
 public:
  explicit inflater() : inflater_impl<Alloc>() {
  }

  ~inflater() noexcept = default;

  inflater(const png::inflater<Alloc>& copy) = delete;
  auto& operator=(const png::inflater<Alloc>& copy) = delete;

  inflater(png::inflater<Alloc>&& move) noexcept = default; // использует ctor от impl
  png::inflater<Alloc>& operator=(png::inflater<Alloc>&& move) noexcept = default; // исп. присваивание от impl

  void flush(png::u8buffer_view new_input) {
    return this->do_flush(new_input);
  }
  void inflate() {
    return this->do_inflate();
  }
  bool done() const {
    return this->do_done();
  }
  auto value() const {
    return static_cast<const_u8buffer_range>(this->do_value());
  }
};

using common_inflater = png::inflater<std::allocator<uint8_t>>;
} // namespace png
} // namespace csc
