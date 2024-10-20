module;

#include <cstdint>
export module csc.png.commons.buffer:range;

export namespace csc {
namespace png {

template <typename It>
struct basic_buffer_range {
  It *m_beg, *m_end;
  It* begin() {
    return m_beg;
  }
  It* end() {
    return m_end;
  }
  const It* begin() const {
    return m_beg;
  }
  const It* end() const {
    return m_end;
  }
  basic_buffer_range() = delete;
  basic_buffer_range(It* b, It* e) : m_beg(b), m_end(e) {
  }
};
using u8buffer_range = png::basic_buffer_range<uint8_t>;
using const_u8buffer_range = png::basic_buffer_range<const uint8_t>;
using s8buffer_range = png::basic_buffer_range<int8_t>;
using const_s8buffer_range = png::basic_buffer_range<const int8_t>;

using u16buffer_range = png::basic_buffer_range<uint16_t>;
using const_u16buffer_range = png::basic_buffer_range<const uint8_t>;
using s16buffer_range = png::basic_buffer_range<int16_t>;
using const_s16buffer_range = png::basic_buffer_range<const int8_t>;

using u32buffer_range = png::basic_buffer_range<uint32_t>;
using const_u32buffer_range = png::basic_buffer_range<const uint32_t>;
using s32buffer_range = png::basic_buffer_range<int32_t>;
using const_s32buffer_range = png::basic_buffer_range<const int32_t>;

using u64buffer_range = png::basic_buffer_range<uint64_t>;
using const_u64buffer_range = png::basic_buffer_range<const uint64_t>;
using s64buffer_range = png::basic_buffer_range<int64_t>;
using const_s64buffer_range = png::basic_buffer_range<const int64_t>;
} // namespace png
} // namespace csc
