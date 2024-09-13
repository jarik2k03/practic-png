module;
#include <cstdint>
export module csc.png.serializer.produce_chunk.buf_writer;
import csc.png.commons.utility.endian;

import :impl;
export import :attributes;

export namespace csc {

class buf_writer : private csc::buf_writer_impl {
 public:
  buf_writer() = delete;
  explicit buf_writer(uint8_t* const s) : csc::buf_writer_impl(s) {
  }
  template <csc::number Val>
  void write(Val val) {
    return this->do_write(val);
  }
};

} // namespace csc
