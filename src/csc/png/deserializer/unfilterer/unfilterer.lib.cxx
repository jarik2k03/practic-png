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

std::vector<uint8_t>
convert_from_4bit_to_8bit(const std::vector<uint8_t>& filtered, uint32_t byteline_count, uint32_t rows_count) {
  std::vector<uint8_t> dst(
      byteline_count * rows_count + rows_count); // байтовое изображение + фильтр-байт на каждой строке

  uint32_t read_offset = 0u, write_offset = 0u;
  for (uint32_t _ = 0u; _ < rows_count; ++_) {
    dst[write_offset] = filtered[read_offset]; // записываем фильтр-байт как есть
    read_offset += 1u, write_offset += 1u;

    constexpr const auto packed_count = 8u / 4u;
    constexpr const auto recon_brightness = 0b11111111u / 0b00001111u;
    for (uint32_t __ = 0u; __ < byteline_count / packed_count; ++__) {
      dst[write_offset + 0u] = ((filtered[read_offset] & 0b11110000) >> 4u) * recon_brightness;
      dst[write_offset + 1u] = ((filtered[read_offset] & 0b00001111) >> 0u) * recon_brightness;

      read_offset += 1u, write_offset += packed_count;
    }
    for (uint32_t pixel = 0u; pixel < byteline_count % packed_count; ++pixel) {
      dst[write_offset] = ((filtered[read_offset] >> (8u - (pixel + 1u) * 4u)) & 0b00001111) * recon_brightness;
      write_offset += 1u;
    } // если так будет, используем последний байт в строке с оставшимися пикселями
    if (byteline_count % packed_count != 0u)
      read_offset += 1u;
  }
  return dst;
}

std::vector<uint8_t>
convert_from_2bit_to_8bit(const std::vector<uint8_t>& filtered, uint32_t byteline_count, uint32_t rows_count) {
  std::vector<uint8_t> dst(
      byteline_count * rows_count + rows_count); // байтовое изображение + фильтр-байт на каждой строке
  uint32_t read_offset = 0u, write_offset = 0u;
  for (uint32_t _ = 0u; _ < rows_count; ++_) {
    dst[write_offset] = filtered[read_offset]; // записываем фильтр-байт как есть
    read_offset += 1u, write_offset += 1u;

    constexpr const auto packed_count = 8u / 2u;
    constexpr const auto recon_brightness = 0b11111111u / 0b00000011u;
    for (uint32_t __ = 0u; __ < byteline_count / packed_count; ++__) {
      dst[write_offset + 0u] = ((filtered[read_offset] & 0b11000000) >> 6u) * recon_brightness;
      dst[write_offset + 1u] = ((filtered[read_offset] & 0b00110000) >> 4u) * recon_brightness;
      dst[write_offset + 2u] = ((filtered[read_offset] & 0b00001100) >> 2u) * recon_brightness;
      dst[write_offset + 3u] = ((filtered[read_offset] & 0b00000011) >> 0u) * recon_brightness;

      read_offset += 1u, write_offset += packed_count;
    }
    for (uint32_t pixel = 0u; pixel < byteline_count % packed_count; ++pixel) {
      dst[write_offset] = ((filtered[read_offset] >> (8u - (pixel + 1u) * 2u)) & 0b00000011) * recon_brightness;
      write_offset += 1u;
    } // если так будет, используем последний байт в строке с оставшимися пикселями
    if (byteline_count % packed_count != 0u)
      read_offset += 1u;
  }
  return dst;
}

std::vector<uint8_t>
convert_from_1bit_to_8bit(const std::vector<uint8_t>& filtered, uint32_t byteline_count, uint32_t rows_count) {
  std::vector<uint8_t> dst(
      byteline_count * rows_count + rows_count); // байтовое изображение + фильтр-байт на каждой строке
  uint32_t read_offset = 0u, write_offset = 0u;
  for (uint32_t _ = 0u; _ < rows_count; ++_) {
    dst[write_offset] = filtered[read_offset]; // записываем фильтр-байт как есть
    read_offset += 1u, write_offset += 1u;

    constexpr const auto packed_count = 8u / 1u;
    constexpr const auto recon_brightness = 0b11111111u / 0b00000001u;
    for (uint32_t __ = 0u; __ < byteline_count / packed_count; ++__) {
      dst[write_offset + 0u] = ((filtered[read_offset] & 0b10000000) >> 7u) * recon_brightness;
      dst[write_offset + 1u] = ((filtered[read_offset] & 0b01000000) >> 6u) * recon_brightness;
      dst[write_offset + 2u] = ((filtered[read_offset] & 0b00100000) >> 5u) * recon_brightness;
      dst[write_offset + 3u] = ((filtered[read_offset] & 0b00010000) >> 4u) * recon_brightness;
      dst[write_offset + 4u] = ((filtered[read_offset] & 0b00001000) >> 3u) * recon_brightness;
      dst[write_offset + 5u] = ((filtered[read_offset] & 0b00000100) >> 2u) * recon_brightness;
      dst[write_offset + 6u] = ((filtered[read_offset] & 0b00000010) >> 1u) * recon_brightness;
      dst[write_offset + 7u] = ((filtered[read_offset] & 0b00000001) >> 0u) * recon_brightness;

      read_offset += 1u, write_offset += packed_count;
    }
    for (uint32_t pixel = 0u; pixel < byteline_count % packed_count; ++pixel) {
      dst[write_offset] = ((filtered[read_offset] >> (8u - (pixel + 1u) * 1u)) & 0b00000001) * recon_brightness;
      write_offset += 1u;
    } // если так будет, используем последний байт в строке с оставшимися пикселями
    if (byteline_count % packed_count != 0u)
      read_offset += 1u;
  }
  return dst;
}

template <uint32_t x_init, uint32_t x_gap, uint32_t y_init, uint32_t y_gap>
class unfilterer {
 private:
  std::vector<uint8_t>& m_read;
  uint32_t m_read_offset = 0u;
  // в обычном случае достаточно указателя на начало строки, но если изображение с битовой глубиной < 8 бит,
  // то здесь будет хранится нормализованная строка С 8-битными пискелями.
  std::vector<uint8_t> m_normalized_readline;

  png::filtering m_algo;
  std::vector<uint8_t> m_current_line;
  std::vector<uint8_t> m_previous_line;

 public:
  unfilterer() = delete;
  unfilterer(std::vector<uint8_t>& read, uint32_t byteline_count, uint32_t pixel_bytesize)
      : m_read(read),
        m_algo(pixel_bytesize, byteline_count),
        m_current_line(byteline_count),
        m_previous_line(byteline_count) {
    m_normalized_readline.resize(byteline_count + 1u);
  }
  std::vector<uint8_t>& unfilter_line();
};

template <uint32_t x_init, uint32_t x_gap, uint32_t y_init, uint32_t y_gap>
std::vector<uint8_t>& unfilterer<x_init, x_gap, y_init, y_gap>::unfilter_line() {
  const auto filter_algo = static_cast<png::e_filter_types>(m_read[m_read_offset]);
  ::memcpy(m_normalized_readline.data(), m_read.data() + m_read_offset + 1u, m_algo.get_linebytes_width());

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
  //    std::cout << " - - READ OFFSET: " << m_read_offset << "; WRITE OFFSET: " << m_write_offset
  //              << "; FILTER: " << +filter_algo << '\n';
  //    std::cout << "WIDTH BYTES: " << m_algo.get_linebytes_width() << " ; PIXEL SIZE: " << m_algo.get_pixel_bytesize()
  //              << '\n';
  m_previous_line = m_current_line; // предыдущая дефильтрованная строка
  m_read_offset +=
      y_init + (m_algo.get_linebytes_width() + sizeof(filter_algo)) * (y_gap - y_init); // учитываем фильтр-байт
  return m_current_line;
}

using unfilterer_0 = png::unfilterer<0u, 1u, 0u, 1u>;
using unfilterer_1 = png::unfilterer<0u, 8u, 0u, 8u>;

} // namespace png
} // namespace csc
