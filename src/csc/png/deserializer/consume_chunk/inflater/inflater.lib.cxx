module;
#include <bits/move.h>
#include <bits/allocator.h>
#include <cstdint>

export module csc.png.deserializer.consume_chunk.inflater;

import :impl;

export namespace csc {

template <typename Alloc>
class inflater : private inflater_impl<Alloc> {
 public:
  explicit inflater() : inflater_impl<Alloc>() {
  }

  ~inflater() noexcept = default;

  inflater(const csc::inflater<Alloc>& copy) = delete;
  auto& operator=(const csc::inflater<Alloc>& copy) = delete;

  inflater(csc::inflater<Alloc>&& move) noexcept = default; // использует ctor от impl
  csc::inflater<Alloc>& operator=(csc::inflater<Alloc>&& move) noexcept = default; // исп. присваивание от impl

  void flush(csc::u8buffer_view new_input) {
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

using common_inflater = csc::inflater<std::allocator<uint8_t>>;
} // namespace csc
