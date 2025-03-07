module;
#include <bit>
#include <cstdint>
#include <cstring>
export module csc.png.serializer.produce_chunk.buf_writer:impl;
export import :attributes;
import csc.png.commons.utility.endian;

namespace csc {
namespace png {

class buf_writer_impl {
 private:
  std::size_t m_pos = 0u;
  uint8_t* const m_start;

 public:
  buf_writer_impl() = delete;
  buf_writer_impl(uint8_t* const s) : m_start(s) {
    static_assert(
        png::is_valid_endian(),
        "This program doesn't support this endian! Your endian is "
        "probably the PDP");
  }
  template <png::number Val>
  void do_write(Val num);
};

template <png::number Val>
void buf_writer_impl::do_write(Val num) {
  if constexpr (sizeof(num) > 1ul) {
    if constexpr (std::endian::native == std::endian::big) {
      // если у нас BE - записываем в пнг-чанк без конвертаций
    } else if constexpr (std::endian::native == std::endian::little) {
      num = png::swap_endian(num); // from LE to BE
    }
  } // используем возможное конвертирование только для 2 и более байтовых чисел

  std::memcpy((m_start + m_pos), &num, sizeof(Val)); // из num в конец буфера
  m_pos += sizeof(num);
}

} // namespace png
} // namespace csc
