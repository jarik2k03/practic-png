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
  const csc::v_sections& m_common_deps;
  const cstd::vector<uint8_t>& m_image_data;

 public:
  f_produce_chunk(csc::chunk& ch, const csc::v_sections& cd, const cstd::vector<uint8_t>& img)
      : m_chunk(ch), m_common_deps(cd), m_image_data(img) {
  }
  // статический полиморфизм
  auto operator()(const csc::IHDR& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::PLTE& b) {
    const csc::IHDR& header = cstd::get<csc::IHDR>(m_common_deps[0]);
    return csc::produce_chunk(b, m_chunk, header);
  }
  auto operator()(const csc::IDAT& b) {
    const csc::IHDR& header = cstd::get<csc::IHDR>(m_common_deps[0]);
    return csc::produce_chunk(b, m_chunk, header, m_image_data);
  }
  auto operator()(const csc::IEND& b) {
    return csc::produce_chunk(b, m_chunk);
  }
  auto operator()(const csc::bKGD& b) {
    const csc::IHDR& header = cstd::get<csc::IHDR>(m_common_deps[0]);
    return csc::produce_chunk(b, m_chunk, header);
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
  auto operator()(const csc::dummy& b) {
    return csc::e_section_code::success;
  }
};

} // namespace csc
