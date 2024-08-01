module;
#include <cstdint>
// #include <iostream>
export module csc.png.png_t.sections.IDAT:impl;

import csc.stl_wrap.vector;
export import csc.png.png_t.sections.IHDR; // зависим от состояния header
export import csc.png.png_t.sections.utils;
export import csc.png.png_t.sections.chunk;
import csc.png.png_t.sections.inflater;

namespace csc {

class IDAT_impl {
 private:
  csc::vector<uint8_t> compressed_data{};
  uint32_t m_crc_adler{0u};

 public:
  IDAT_impl() = default;
  csc::section_code_t do_construct(const csc::chunk& raw, const csc::IHDR& header) noexcept;
};

csc::section_code_t IDAT_impl::do_construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
  // TO DO: сделать дифлейту
  csc::inflater infstream(raw.data);
  infstream.inflate(0);

  m_crc_adler = raw.crc_adler;
  // std::cout << "IDAT chunk size: " << raw.contained_length << '\n';
  return csc::section_code_t::success;
}

} // namespace csc
