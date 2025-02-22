module;

#include <cstdint>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

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
  png::cHRM* colorspace_status() {
    const auto is_this_section = [](png::v_section& cur) { return std::get_if<png::cHRM>(&cur) != nullptr; };
    auto cHRM_iterator = std::ranges::find_if(m_structured, is_this_section);
    return (cHRM_iterator != m_structured.end()) ? &std::get<png::cHRM>(*cHRM_iterator) : nullptr;
  }

  const png::IHDR& header() const {
    return std::get<png::IHDR>(m_structured.at(0u));
  }
  png::IHDR& header() {
    return std::get<png::IHDR>(m_structured.at(0u));
  }
};

} // namespace png
} // namespace csc
