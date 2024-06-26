module;
#include <cstdint>

#include <array>
#include <variant>
#include <iostream>
#include <vector>
export module csc.png.png_t.sections;

export import :chunk;
export import :utils;
export import :attributes;

export namespace csc {

using v_section = std::variant<class IHDR, class PLTE, class IDAT, class IEND>;
using v_sections = std::vector<v_section>;

struct SUBSCRIBE {
  const std::array<uint8_t, 8> data = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
  bool operator==(const csc::SUBSCRIBE& other) {
    return this->data == other.data;
  }
  bool operator!=(const csc::SUBSCRIBE& other) {
    return this->data != other.data;
  }
};

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
  uint32_t construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept;
};

class PLTE {
 private:
  struct rgb8 {
    uint8_t r, g, b;
  };
  std::vector<rgb8> full_palette;
  uint8_t full_palette_size = 0;
  uint32_t m_crc_adler;

 public:
  uint32_t construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept;
};

class IDAT {
 private:
  std::vector<uint8_t> compressed_data;
  uint32_t m_crc_adler;

 public:
  uint32_t construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept;
};
class IEND {
 private:
  uint32_t m_crc_adler;

 public:
  uint32_t construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept;
};

} // namespace csc

namespace csc {
uint32_t IHDR::construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept {
  uint32_t buf_pos = 0u; // для header не требуются зависимые сектора
  csc::read_var_from_vector_swap(m_width, buf_pos, raw.data), buf_pos += sizeof(m_width);
  csc::read_var_from_vector_swap(m_height, buf_pos, raw.data), buf_pos += sizeof(m_height);
  csc::read_var_from_vector(m_bit_depth, buf_pos, raw.data), buf_pos += sizeof(m_bit_depth);
  csc::read_var_from_vector(m_color_type, buf_pos, raw.data), buf_pos += sizeof(m_color_type);
  csc::read_var_from_vector(m_compression, buf_pos, raw.data), buf_pos += sizeof(m_compression);
  csc::read_var_from_vector(m_filter, buf_pos, raw.data), buf_pos += sizeof(m_filter);
  csc::read_var_from_vector(m_interlace, buf_pos, raw.data), buf_pos += sizeof(m_interlace);
  m_crc_adler = raw.crc_adler;
  return 0u;
}
uint32_t PLTE::construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept {
  const csc::IHDR& header = std::get<csc::IHDR>(deps[0]);
  if (raw.contained_length % 3 != 0 || raw.contained_length > (256 * 3))
    return 1u;
  if (header.color_type() == color_type_t::indexed && raw.contained_length / 3 > (1 << header.bit_depth())) 
    return 1u;

  m_crc_adler = raw.crc_adler;
  return 0u;
}
uint32_t IDAT::construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept {
  return 0u;
}
uint32_t IEND::construct(const csc::chunk& raw, const csc::v_sections& deps) noexcept {
  return 0u;
}

} // namespace csc

export namespace csc {

struct f_construct {
  const csc::chunk& m_chunk;
  const csc::v_sections& m_common_deps;
  f_construct(const csc::chunk& ch, const csc::v_sections& cd) : m_chunk(ch), m_common_deps(cd) {
  }
  // статический полиморфизм
  uint32_t operator()(csc::IHDR& b) {
    return b.construct(m_chunk, m_common_deps);
  }
  uint32_t operator()(csc::PLTE& b) {
    return b.construct(m_chunk, m_common_deps);
  }
  uint32_t operator()(csc::IDAT& b) {
    return b.construct(m_chunk, m_common_deps);
  }
  uint32_t operator()(csc::IEND& b) {
    return b.construct(m_chunk, m_common_deps);
  }
};

} // namespace csc
