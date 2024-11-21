module;
#include <cstdint>
#include <iostream>
export module csc.png.deserializer.unfilterer;

export import csc.png.commons.buffer_view;
export import csc.png.commons.buffer;
import stl.stdexcept;

export namespace csc {
namespace png {

enum e_filter_types : uint8_t {
  none = 0u,
  sub = 1u,
  up = 2u,
  average = 3u,
  paeth = 4u
};


class filtering {
 private:
  uint32_t m_pixel_bytesize;
  uint32_t m_linebytes_width;
 public:
  filtering() = delete;
  filtering(uint32_t pixel_bytesize, uint32_t linebytes_width) : m_pixel_bytesize(pixel_bytesize), m_linebytes_width(linebytes_width) {}
  uint32_t get_linebytes_width() const noexcept;
  uint32_t get_pixel_bytesize() const noexcept;

  void none(uint8_t* to, const uint8_t* current) const noexcept;
  void sub(uint8_t* to, const uint8_t* current) const noexcept;
  void up(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void average(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void paeth(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
};

uint32_t filtering::get_linebytes_width() const noexcept {
  return m_linebytes_width;
}
uint32_t filtering::get_pixel_bytesize() const noexcept {
  return m_pixel_bytesize;
}


void filtering::none(uint8_t* to, const uint8_t* current) const noexcept {
  for (uint32_t i = 0u; i < m_linebytes_width; ++i) {
    to[i] = current[i];
  }
}

void filtering::sub(uint8_t* to, const uint8_t* current) const noexcept {
  // for (uint32_t i = 0u; i < m_linebytes_width; ++i) {
  //   to[i] = current[i];
  // }
}



class unfilterer {
 private:
  png::u8buffer_view m_filtered;
  png::u8buffer& m_write;
  const uint8_t* m_previous_line = nullptr;
  uint32_t m_read_offset = 0u;
  uint32_t m_write_offset = 0u;
  png::filtering m_algo;

 public:
  unfilterer() = delete;
  unfilterer(png::u8buffer_view filtered, png::u8buffer& to_write, uint32_t byteline_count, uint32_t pixel_bytesize)
      : m_filtered(filtered), m_write(to_write), m_algo(pixel_bytesize, byteline_count) {
  }
  void unfilter_line();
};

void unfilterer::unfilter_line() {
  const auto filter_algo = static_cast<png::e_filter_types>(m_filtered.data()[m_read_offset]);
  if (filter_algo != e_filter_types::none && filter_algo != e_filter_types::sub && m_previous_line == nullptr)
    throw std::domain_error(
        "Расжатое изображение использует на текущей строке фильтр, требующий предыдущую строку, но она не определена. "
        "(Возможно, это первая строка. )");
  switch (filter_algo) {
    case e_filter_types::none:
      m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::sub:
      m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      // m_algo.sub(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::up:
      m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::average:
      m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::paeth:
      m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    default:
      // throw std::domain_error("Неизвестный тип фильтра. Дефильтровать невозможно.");
      break;
  }
  std::cout << " - - READ OFFSET: " << m_read_offset << "; WRITE OFFSET: " << m_write_offset << "; FILTER: " << +filter_algo << '\n';
  std::cout << "WIDTH BYTES: " << m_algo.get_linebytes_width() << " ; PIXEL SIZE: " << m_algo.get_pixel_bytesize() << '\n';
  m_previous_line = m_filtered.data() + m_read_offset + sizeof(filter_algo); // только данные для дефильтрации
  m_read_offset += m_algo.get_linebytes_width() + sizeof(filter_algo); // учитываем фильтр-байт
  m_write_offset += m_algo.get_linebytes_width();
}

} // namespace png
} // namespace csc
