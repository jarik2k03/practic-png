module;
#include <cstdint>
#include <iostream>
#include <cstring>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
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

constexpr int32_t average_arithmetic(int32_t a, int32_t b) noexcept {
  return (a + b) >> 1u;
}

constexpr uint8_t paeth_predictor(uint8_t a, uint8_t b, uint8_t c) noexcept {
  int32_t p = a + b - c;
  const int32_t pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
  if (pa <= pb && pa <= pc)
    return a;
  else if (pb <= pc)
    return b;
  else
    return c;
}

class filtering {
 private:
  uint32_t m_pixel_bytesize;
  uint32_t m_linebytes_width;

 public:
  filtering() = delete;
  filtering(uint32_t pixel_bytesize, uint32_t linebytes_width)
      : m_pixel_bytesize(pixel_bytesize), m_linebytes_width(linebytes_width) {
  }
  uint32_t get_linebytes_width() const noexcept;
  uint32_t get_pixel_bytesize() const noexcept;

  void none(uint8_t* to, const uint8_t* current) const noexcept;
  void sub(uint8_t* to, const uint8_t* current) const noexcept;
  void up(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void average(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void average(uint8_t* to, const uint8_t* current) const noexcept; // частный случай
  void paeth(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void paeth(uint8_t* to, const uint8_t* current) const noexcept;
};

uint32_t filtering::get_linebytes_width() const noexcept {
  return m_linebytes_width;
}
uint32_t filtering::get_pixel_bytesize() const noexcept {
  return m_pixel_bytesize;
}

void filtering::none(uint8_t* to, const uint8_t* current) const noexcept {
  ::memcpy(to, current, m_linebytes_width); // копируем линию
}

void filtering::sub(uint8_t* to, const uint8_t* current) const noexcept {
  ::memcpy(to, current, m_pixel_bytesize); // копируем первый пиксель
  for (uint32_t i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    to[i] = current[i] + recon_a;
  }
}

void filtering::up(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept {
  for (auto i = 0u; i < m_linebytes_width; ++i) {
    const auto recon_b = previous[i];
    to[i] = current[i] + recon_b;
  }
}

void filtering::average(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    const auto recon_b = previous[i];
    to[i] = current[i] + average_arithmetic(0u, recon_b);
  } // первый пиксель в строке не учитывает предыдущие

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    const auto recon_b = previous[i];
    to[i] = current[i] + static_cast<uint8_t>(png::average_arithmetic(recon_a, recon_b));
  }
}

void filtering::average(uint8_t* to, const uint8_t* current) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    to[i] = current[i] + average_arithmetic(0u, 0u);
  } // первый пиксель в строке не учитывает предыдущие

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    to[i] = current[i] + static_cast<uint8_t>(png::average_arithmetic(recon_a, 0u)); // recon_b замещает пустой байт
  }
}

void filtering::paeth(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    const auto recon_b = previous[i];
    to[i] = current[i] + png::paeth_predictor(0u, recon_b, 0u);
  } // первый пиксель в строке не учитывает предыдущие

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    const auto recon_b = previous[i];
    const auto recon_c = previous[i - m_pixel_bytesize];

    to[i] = current[i] + png::paeth_predictor(recon_a, recon_b, recon_c);
  }
}

void filtering::paeth(uint8_t* to, const uint8_t* current) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    to[i] = current[i] + png::paeth_predictor(0u, 0u, 0u);
  } // первый пиксель в строке не учитывает ничего

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];

    to[i] = current[i] + png::paeth_predictor(recon_a, 0u, 0u); // нули замещают recon_b и recon_c
  }
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
  switch (filter_algo) {
    case e_filter_types::none:
      m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::sub:
      m_algo.sub(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::up:
      [[likely]] if (m_previous_line != nullptr)
        m_algo.up(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u, m_previous_line);
      else
        m_algo.none(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::average:
      [[likely]] if (m_previous_line != nullptr)
        m_algo.average(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u, m_previous_line);
      else
        m_algo.average(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    case e_filter_types::paeth:
      [[likely]] if (m_previous_line != nullptr)
        m_algo.paeth(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u, m_previous_line);
      else
        m_algo.paeth(m_write.data() + m_write_offset, m_filtered.data() + m_read_offset + 1u);
      break;
    default:
      throw std::domain_error("Неизвестный тип фильтра. Дефильтровать невозможно.");
      break;
  }
  std::cout << " - - READ OFFSET: " << m_read_offset << "; WRITE OFFSET: " << m_write_offset
            << "; FILTER: " << +filter_algo << '\n';
  std::cout << "WIDTH BYTES: " << m_algo.get_linebytes_width() << " ; PIXEL SIZE: " << m_algo.get_pixel_bytesize()
            << '\n';
  m_previous_line = m_write.data() + m_write_offset; // предыдущая дефильтрованная строка
  m_read_offset += m_algo.get_linebytes_width() + sizeof(filter_algo); // учитываем фильтр-байт
  m_write_offset += m_algo.get_linebytes_width();
}

} // namespace png
} // namespace csc
