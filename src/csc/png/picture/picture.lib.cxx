module;

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <ranges>

export module csc.png.picture;
export import :signature;

export import csc.png.picture.sections;

import stl.stl_wrap.variant;
import stl.stl_wrap.vector;

export namespace csc {

class picture {
 public:
  csc::png_signature m_start;
  csc::v_sections m_structured{};
  std::vector<uint8_t> m_image_data{};
  csc::IHDR m_header;
  csc::IEND m_eof_block;

 public:
  picture() = default;

  const csc::png_signature& start() const {
    return m_start;
  }
  const csc::IHDR& header() const {
    return m_header;
  }
  const csc::IEND& eof_block() const {
    return m_eof_block;
  }

  csc::png_signature& start() {
    return m_start;
  }
  csc::IHDR& header() {
    return m_header;
  }
  csc::IEND& eof_block() {
    return m_eof_block;
  }
};

} // namespace csc
