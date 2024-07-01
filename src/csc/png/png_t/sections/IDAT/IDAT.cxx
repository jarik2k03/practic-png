module;
#include <cstdint>
#include <vector>
export module csc.png.png_t.sections.IDAT;

import csc.png.png_t.sections.IHDR; // зависим от состояния header
import csc.png.png_t.sections.chunk;
import csc.png.png_t.sections.utils;
import csc.png.png_t.sections.inflate;

export namespace csc {


class IDAT {
 private:
  std::vector<uint8_t> compressed_data;
  uint32_t m_crc_adler;

 public:
  csc::section_code_t construct(const csc::chunk& raw, const csc::IHDR& header) noexcept;
};

csc::section_code_t IDAT::construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
  // TO DO: сделать дифлейту
  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;

}


}
