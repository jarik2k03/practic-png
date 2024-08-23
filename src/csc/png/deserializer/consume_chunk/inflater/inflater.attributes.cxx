module;
#include <cstdint>
export module csc.png.deserializer.consume_chunk.inflater:attributes;

export namespace csc {

struct uncompressed_range {
  uint8_t *m_beg, *m_end;
  uint8_t* begin() {
    return m_beg;
  }
  uint8_t* end() {
    return m_end;
  }
  uncompressed_range() = delete;
  uncompressed_range(uint8_t* b, uint8_t* e) : m_beg(b), m_end(e) {
  }
};

} // namespace csc
