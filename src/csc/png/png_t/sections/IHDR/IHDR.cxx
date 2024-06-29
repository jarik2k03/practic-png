module;
#include <cstdint>
export module csc.png.png_t.sections.IHDR;

export import :attributes;
import csc.png.png_t.sections.chunk;
import csc.png.png_t.sections.utils;

export namespace csc {
class IHDR {
 private:
  uint32_t m_width, m_height;
  uint8_t m_bit_depth;
  csc::color_type_t m_color_type;
  csc::compression_t m_compression;
  csc::filter_t m_filter;
  csc::interlace_t m_interlace;
  uint32_t m_crc_adler;

 public:
  uint32_t width() const noexcept {
    return m_width;
  }
  uint32_t height() const noexcept {
    return m_height;
  }
  uint8_t bit_depth() const noexcept {
    return m_bit_depth;
  }
  csc::color_type_t color_type() const noexcept {
    return m_color_type;
  }
  csc::compression_t compression() const noexcept {
    return m_compression;
  }
  csc::filter_t filter() const noexcept {
    return m_filter;
  }
  csc::interlace_t interlace() const noexcept {
    return m_interlace;
  }
  uint32_t crc_adler() const noexcept {
    return m_crc_adler;
  }
  csc::section_code_t construct(const csc::chunk& raw) noexcept;
};

csc::section_code_t IHDR::construct(const csc::chunk& raw) noexcept {
  uint32_t buf_pos = 0u; // для header не требуются зависимые сектора
  csc::read_var_from_vector_swap(m_width, buf_pos, raw.data), buf_pos += sizeof(m_width);
  csc::read_var_from_vector_swap(m_height, buf_pos, raw.data), buf_pos += sizeof(m_height);
  csc::read_var_from_vector(m_bit_depth, buf_pos, raw.data), buf_pos += sizeof(m_bit_depth);
  csc::read_var_from_vector(m_color_type, buf_pos, raw.data), buf_pos += sizeof(m_color_type);
  csc::read_var_from_vector(m_compression, buf_pos, raw.data), buf_pos += sizeof(m_compression);
  csc::read_var_from_vector(m_filter, buf_pos, raw.data), buf_pos += sizeof(m_filter);
  csc::read_var_from_vector(m_interlace, buf_pos, raw.data), buf_pos += sizeof(m_interlace);
  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}


}
