module;
#include <cstdint>
// #include <bits/stl_iterator.h>
export module csc.png.png_t.sections.PLTE:impl;

import csc.stl_wrap.vector;
export import csc.png.png_t.sections.IHDR; // зависим от состояния header
export import csc.png.png_t.sections.chunk;
export import csc.png.png_t.sections.utils;

namespace csc {

class PLTE_impl {
 private:
  struct rgb8 {
    uint8_t r, g, b;
  };
  csc::vector<rgb8> full_palette;
  uint32_t m_crc_adler;

 public:
  csc::section_code_t do_construct(const csc::chunk& raw, const csc::IHDR& header) noexcept;
};

csc::section_code_t PLTE_impl::do_construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
  if (raw.data.size() % 3 != 0 || raw.data.size() > (256 * 3))
    return csc::section_code_t::error;
  if (header.color_type() == color_type_t::indexed && raw.data.size() / 3 > (1 << header.bit_depth()))
    return csc::section_code_t::error;
  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}

} // namespace csc
