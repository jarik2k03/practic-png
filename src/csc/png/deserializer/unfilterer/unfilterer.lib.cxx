module;
#include <cstdint>
#include <iostream>
#include <cstring>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
export module csc.png.deserializer.unfilterer;

export import csc.png.commons.buffer_view;
export import csc.png.commons.buffer;
export import csc.png.commons.filtering;
import stl.stdexcept;
import stl.vector;

export namespace csc {
namespace png {


void normalize_readline(uint8_t* dst, const uint8_t* src, uint32_t size, uint8_t src_bit_depth) {
    const uint32_t packed_count = (8u / std::clamp(static_cast<uint32_t>(src_bit_depth), 0u, 8u));
    for (uint32_t idx = 0u; idx < size; ++idx) {
        const uint8_t px = src[idx];
        const uint32_t p = idx * packed_count;
        switch (src_bit_depth) {
            case 1:
             dst[p + 0] = (px >> 7 & 0x1) * 255, dst[p + 1] = (px >> 6 & 0x1) * 255;
             dst[p + 2] = (px >> 5 & 0x1) * 255, dst[p + 3] = (px >> 4 & 0x1) * 255;
             dst[p + 4] = (px >> 3 & 0x1) * 255, dst[p + 5] = (px >> 2 & 0x1) * 255;
             dst[p + 6] = (px >> 1 & 0x1) * 255, dst[p + 7] = (px >> 0 & 0x1) * 255;
             break;
            case 2:
             dst[p + 0] = (px >> 6 & 0b0011) * 64, dst[p + 1] = (px >> 4 & 0b0011) * 64;
             dst[p + 2] = (px >> 2 & 0b0011) * 64, dst[p + 3] = (px >> 0 & 0b0011) * 64;
             break;
            case 4:
             dst[p + 0] = (px >> 4 & 0b1111) * 16, dst[p + 1] = (px >> 0 & 0b1111) * 16;
             break;
            default:
             dst[p + 0] = px;
             break;
        }
    }
}

class unfilterer {
 private:
  std::vector<uint8_t>& m_read;
  uint32_t m_read_offset = 0u;
  uint8_t m_bit_depth = 0u;
  // в обычном случае достаточно указателя на начало строки, но если изображение с битовой глубиной < 8 бит,
  // то здесь будет хранится нормализованная строка С 8-битными пискелями.
  std::vector<uint8_t> m_normalized_readline;

  png::filtering m_algo;
  std::vector<uint8_t> m_current_line;
  std::vector<uint8_t> m_previous_line;

 public:
  unfilterer() = delete;
  unfilterer(std::vector<uint8_t>& read, uint8_t bit_depth) : m_read(read), m_bit_depth(bit_depth) {}
  unfilterer(std::vector<uint8_t>& read, uint32_t byteline_count, uint32_t pixel_bytesize, uint8_t bit_depth)
      : m_read(read),
        m_bit_depth(bit_depth),
        m_normalized_readline(byteline_count + 1u),
        m_algo(pixel_bytesize, byteline_count),
        m_current_line(byteline_count),
        m_previous_line(byteline_count) {
  }
  void switch_context(uint32_t byteline_count, uint32_t pixel_bytesize) {
    m_algo = png::filtering(pixel_bytesize, byteline_count);
    m_normalized_readline.resize(byteline_count + 1u);
    m_normalized_readline.reserve(byteline_count + 1u);
    m_current_line.resize(byteline_count);
    m_current_line.reserve(byteline_count + 1);
    m_previous_line.resize(byteline_count);
    m_previous_line.reserve(byteline_count + 1);
  }
  std::vector<uint8_t>& unfilter_line();
};

std::vector<uint8_t>& unfilterer::unfilter_line() {
  const auto filter_algo = static_cast<png::e_filter_types>(m_read[m_read_offset]);
  png::normalize_readline(m_normalized_readline.data(), m_read.data() + m_read_offset + 1u, m_algo.get_linebytes_width(), m_bit_depth);

  switch (filter_algo) {
    case e_filter_types::none:
      m_algo.none(m_current_line.data(), m_normalized_readline.data());
      break;
    case e_filter_types::sub:
      m_algo.sub(m_current_line.data(), m_normalized_readline.data());
      break;
    case e_filter_types::up:
      [[likely]] if (!m_previous_line.empty())
        m_algo.up(m_current_line.data(), m_normalized_readline.data(), m_previous_line.data());
      else
        m_algo.none(m_current_line.data(), m_normalized_readline.data());
      break;
    case e_filter_types::average:
      [[likely]] if (!m_previous_line.empty())
        m_algo.average(m_current_line.data(), m_normalized_readline.data(), m_previous_line.data());
      else
        m_algo.average(m_current_line.data(), m_normalized_readline.data());
      break;
    case e_filter_types::paeth:
      [[likely]] if (!m_previous_line.empty())
        m_algo.paeth(m_current_line.data(), m_normalized_readline.data(), m_previous_line.data());
      else
        m_algo.paeth(m_current_line.data(), m_normalized_readline.data());
      break;
    default:
      throw std::domain_error("Неизвестный тип фильтра. Дефильтровать невозможно.");
      break;
  }
  ::memcpy(m_previous_line.data(), m_current_line.data(), m_current_line.size()); // предыдущая дефильтрованная строка
  m_read_offset += (m_algo.get_linebytes_width() + sizeof(filter_algo)); // учитываем фильтр-байт
  return m_current_line;
}

} // namespace png
} // namespace csc
