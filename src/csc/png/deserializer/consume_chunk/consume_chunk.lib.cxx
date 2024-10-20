module;
#include <cstdint>
#include <bits/stl_iterator.h>
export module csc.png.deserializer.consume_chunk;

import :utility;
import :overloads;

export import csc.png.commons.chunk;
import stl.variant;

export namespace csc {
namespace png {

class f_consume_chunk {
 private:
  const png::chunk& m_chunk;
  const png::v_sections& m_common_deps;

 public:
  constexpr f_consume_chunk(const png::chunk& ch, const png::v_sections& cd) : m_chunk(ch), m_common_deps(cd) {
  }
  // статический полиморфизм
  auto operator()(png::IHDR& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::PLTE& b) {
    const auto header_pos = m_common_deps.cbegin();
    if (header_pos == m_common_deps.cend())
      return png::e_section_code::depends_ihdr; // не нашли зависимость
    const png::IHDR* header = std::get_if<png::IHDR>(&*header_pos);
    if (header == nullptr) // не удалось получить нужную зависимость
      return png::e_section_code::depends_ihdr;
    return png::consume_chunk(b, m_chunk, *header);
  }
  auto operator()(png::IEND& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::bKGD& b) {
    const auto header_pos = m_common_deps.cbegin();
    if (header_pos == m_common_deps.cend())
      return png::e_section_code::depends_ihdr; // не нашли зависимость
    const png::IHDR* header = std::get_if<png::IHDR>(&*header_pos);
    if (header == nullptr) // не удалось получить нужную зависимость
      return png::e_section_code::depends_ihdr;
    return png::consume_chunk(b, m_chunk, *header);
  }
  auto operator()(png::tIME& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::cHRM& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::gAMA& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::hIST& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::pHYs& b) {
    return png::consume_chunk(b, m_chunk);
  }
  auto operator()(png::tRNS& b) {
    const auto header_pos = m_common_deps.cbegin();
    if (header_pos == m_common_deps.cend())
      return png::e_section_code::depends_ihdr; // не нашли зависимость
    const png::IHDR* header = std::get_if<png::IHDR>(&*header_pos);
    if (header == nullptr) // не удалось получить нужную зависимость
      return png::e_section_code::depends_ihdr;
    return png::consume_chunk(b, m_chunk, *header);
  }
};

} // namespace png
} // namespace csc
