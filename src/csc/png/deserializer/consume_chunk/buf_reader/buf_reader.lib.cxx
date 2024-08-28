module;
#include <cstdint>
export module csc.png.deserializer.consume_chunk.buf_reader;

import :impl;
export import :attributes;

export namespace csc {

class buf_reader : private csc::buf_reader_impl {
 public:
  buf_reader() = delete;
  buf_reader(const uint8_t* const s) : csc::buf_reader_impl(s) {
  }
  template <csc::number Val>
  Val read() {
    return this->do_read<Val>();
  }
};

} // namespace csc
