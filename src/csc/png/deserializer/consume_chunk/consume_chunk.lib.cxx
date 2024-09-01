module;
#include <cstdint>
export module csc.png.deserializer.consume_chunk;

import :utility;
import :overloads;

export import csc.png.commons.chunk;

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
    const csc::IHDR& header = cstd::get<csc::IHDR>(m_common_deps[0]);
    return csc::consume_chunk(b, m_chunk, header);
  }
  auto operator()(csc::IEND& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::bKGD& b) {
    const csc::IHDR& header = cstd::get<csc::IHDR>(m_common_deps[0]);
    return csc::consume_chunk(b, m_chunk, header);
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
