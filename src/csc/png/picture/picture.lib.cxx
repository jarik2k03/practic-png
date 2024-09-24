module;

#include <cstdint>

export module csc.png.picture;
export import :signature;

export import csc.png.picture.sections;

import stl.variant;
import stl.vector;

export namespace csc {
namespace png {

class picture {
 public:
  png::png_signature m_start;
  png::v_sections m_structured{};
  std::vector<uint8_t> m_image_data{};
  png::IHDR m_header;
  png::IEND m_eof_block;

 public:
  picture() = default;

  const png::png_signature& start() const {
    return m_start;
  }
  const png::IHDR& header() const {
    return m_header;
  }
  const png::IEND& eof_block() const {
    return m_eof_block;
  }

  png::png_signature& start() {
    return m_start;
  }
  png::IHDR& header() {
    return m_header;
  }
  png::IEND& eof_block() {
    return m_eof_block;
  }
};

} // namespace png
} // namespace csc
