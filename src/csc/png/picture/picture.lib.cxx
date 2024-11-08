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

 public:
  picture() = default;

  const png::png_signature& start() const {
    return m_start;
  }
  png::png_signature& start() {
    return m_start;
  }
  const png::IHDR& header() const {
    return std::get<png::IHDR>(m_structured.at(0u));
  }
};

} // namespace png
} // namespace csc
