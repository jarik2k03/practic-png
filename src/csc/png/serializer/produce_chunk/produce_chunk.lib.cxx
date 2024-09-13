module;
#include <cstdint>
export module csc.png.serializer.produce_chunk;

import :utility;
import :overloads;

export import csc.png.commons.chunk;

export namespace csc {

class f_produce_chunk {
 private:
  csc::chunk& m_chunk;

 public:
  constexpr f_produce_chunk(csc::chunk& ch) : m_chunk(ch) {
  }
  // статический полиморфизм
  auto operator()(const csc::IHDR& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::PLTE& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::IEND& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::bKGD& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::tIME& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::cHRM& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::gAMA& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::hIST& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::pHYs& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::dummy&) {
    return csc::e_section_code::success;
  }
};

} // namespace csc
