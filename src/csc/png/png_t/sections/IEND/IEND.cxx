module;
#include <cstdint>
export module csc.png.png_t.sections.IEND;

import csc.png.png_t.sections.chunk;
import csc.png.png_t.sections.utils;

export namespace csc {
class IEND {
 private:
  uint32_t m_crc_adler;

 public:
  csc::section_code_t construct(const csc::chunk& raw) noexcept;
};

csc::section_code_t IEND::construct(const csc::chunk& raw) noexcept {
  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}





}
