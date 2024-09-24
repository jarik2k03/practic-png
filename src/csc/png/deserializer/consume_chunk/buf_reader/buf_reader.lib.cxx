module;
#include <cstdint>
export module csc.png.deserializer.consume_chunk.buf_reader;
import csc.png.commons.utility.endian;

import :impl;
export import :attributes;

export namespace csc {
namespace png {

class buf_reader : private png::buf_reader_impl {
 public:
  buf_reader() = delete;
  explicit buf_reader(const uint8_t* const s) : png::buf_reader_impl(s) {
  }
  template <png::number Val>
  Val read() {
    return this->do_read<Val>();
  }
};

} // namespace png
} // namespace csc
