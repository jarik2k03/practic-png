module;
#include <cstdint>
// #include <bits/stl_iterator.h>
#include <algorithm>
#include <ranges>
export module csc.png.png_t.sections.PLTE:impl;

import csc.stl_wrap.vector;
export import csc.png.png_t.sections.IHDR; // зависим от состояния header
export import csc.png.png_t.sections.chunk;
export import csc.png.png_t.sections.utils;

export import :attributes;
namespace csc {

class PLTE_impl {
 private:
  csc::vector<rgb8> m_full_palette;
  uint32_t m_crc_adler;

 public:
  const csc::vector<rgb8>& do_full_palette() const;
  csc::section_code_t do_construct(const csc::chunk& raw, const csc::IHDR& header) noexcept;
};

const csc::vector<rgb8>& PLTE_impl::do_full_palette() const {
  return m_full_palette;
}
csc::section_code_t PLTE_impl::do_construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
  if (raw.data.size() % 3 != 0 || raw.data.size() > (256 * 3))
    return csc::section_code_t::error;
  if (header.color_type() == color_type_t::indexed && raw.data.size() / 3 > (1 << header.bit_depth()))
    return csc::section_code_t::error;
  m_full_palette.reserve(256u);

  for (uint32_t pos = 0u; pos < raw.data.size(); pos += 3u) {
    rgb8 plte_unit;
    csc::read_var_from_vector(plte_unit, pos, raw.data);
    m_full_palette.emplace_back(std::move(plte_unit));
  }

  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}

} // namespace csc
