module;
#include <cstdint>
#include <bits/stl_iterator.h>
export module csc.png.deserializer.consume_chunk;

import :utility;
import :overloads;

export import csc.png.commons.chunk;
import cstd.stl_wrap.variant;

export namespace csc {

class f_consume_chunk {
 private:
  const csc::chunk& m_chunk;
  const csc::v_sections& m_common_deps;

 public:
  f_consume_chunk(const csc::chunk& ch, const csc::v_sections& cd) : m_chunk(ch), m_common_deps(cd) {
  }
  // статический полиморфизм
  auto operator()(csc::IHDR& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::PLTE& b) {
    const auto header_pos = m_common_deps.cbegin();
    if (header_pos == m_common_deps.cend())
      return csc::e_section_code::depends_ihdr; // не нашли зависимость
    const csc::IHDR* header = cstd::get_if<csc::IHDR>(&*header_pos);
    if (header == nullptr) // не удалось получить нужную зависимость
      return csc::e_section_code::depends_ihdr;
    return csc::consume_chunk(b, m_chunk, *header);
  }
  auto operator()(csc::IEND& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::bKGD& b) {
    const auto header_pos = m_common_deps.cbegin();
    if (header_pos == m_common_deps.cend())
      return csc::e_section_code::depends_ihdr; // не нашли зависимость
    const csc::IHDR* header = cstd::get_if<csc::IHDR>(&*header_pos);
    if (header == nullptr) // не удалось получить нужную зависимость
      return csc::e_section_code::depends_ihdr;
    return csc::consume_chunk(b, m_chunk, *header);
  }
  auto operator()(csc::tIME& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::cHRM& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::gAMA& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::hIST& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::pHYs& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::dummy&) {
    return csc::e_section_code::success;
  }
};

} // namespace csc
