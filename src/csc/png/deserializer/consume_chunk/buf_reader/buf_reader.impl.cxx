module;
#include <cstdint>
#include <type_traits>
#include <bit>
#include <cstring>
module csc.png.deserializer.consume_chunk.buf_reader:impl;

export import :attributes;

namespace csc {

class buf_reader_impl {
  private:
    std::size_t m_pos = 0u;
    const uint8_t* const m_start;
  public:
    buf_reader_impl() = delete;
    buf_reader_impl(const uint8_t* const s) : m_start(s) {
      static_assert(csc::is_valid_endian(), "This program doesn't support this endian! Your endian is probably the PDP");
    }
    template <csc::number Val>
    Val do_read();

};

template <csc::number Val>
Val buf_reader_impl::do_read() {
  Val word;
  std::memcpy(&word, m_start + m_pos, sizeof(Val)); 
  m_pos += sizeof(word);
  if constexpr (sizeof(word) == 1ul) {
    return word; // при чтении одного байта - порядок байт не важен
  }

  if constexpr (std::endian::native == std::endian::big) {
    word = csc::swap_endian(word); // from LE to BE 
  }
  else if constexpr (std::endian::native == std::endian::little) {
   // если на нашем компьютере LE - читаем напрямую (png использует Little Endian) 
  }
  return word;
}




}

