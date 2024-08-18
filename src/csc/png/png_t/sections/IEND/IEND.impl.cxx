module;
#include <cstdint>
export module csc.png.png_t.sections.IEND:impl;

import csc.stl_wrap.iostream;
export import csc.png.png_t.sections.chunk;
export import csc.png.png_t.sections.utils;

namespace csc {
class IEND_impl {
 private:
  uint32_t m_crc_adler;

 public:
  csc::section_code_t do_construct(const csc::chunk& raw) noexcept;
};

csc::section_code_t IEND_impl::do_construct(const csc::chunk& raw) noexcept {
  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}

} // namespace csc
