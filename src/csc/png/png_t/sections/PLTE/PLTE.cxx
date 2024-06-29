module;
#include <cstdint>
#include <vector>
export module csc.png.png_t.sections.PLTE;

import csc.png.png_t.sections.IHDR; // зависим от состояния header
import csc.png.png_t.sections.chunk;
import csc.png.png_t.sections.utils;

export namespace csc {

class PLTE {
 private:
  struct rgb8 {
    uint8_t r, g, b;
  };
  std::vector<rgb8> full_palette;
  uint32_t m_crc_adler;

 public:
  csc::section_code_t construct(const csc::chunk& raw, const csc::IHDR& header) noexcept;
};

csc::section_code_t PLTE::construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
  if (raw.contained_length % 3 != 0 || raw.contained_length > (256 * 3))
    return csc::section_code_t::error;
  if (header.color_type() == color_type_t::indexed && raw.contained_length / 3 > (1 << header.bit_depth())) 
    return csc::section_code_t::error;


  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}

}
