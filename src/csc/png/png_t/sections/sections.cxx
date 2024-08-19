module;
#include <cstdint>
export module csc.png.png_t.sections;

import csc.stl_wrap.vector;
import csc.stl_wrap.array;
import csc.stl_wrap.variant;

export import csc.png.png_t.sections.chunk;
export import csc.png.png_t.sections.utils;
import csc.png.png_t.sections.inflater;

export import csc.png.png_t.sections.IHDR;
export import csc.png.png_t.sections.PLTE;
export import csc.png.png_t.sections.IDAT;
export import csc.png.png_t.sections.IEND;

export namespace csc {

using v_section = csc::variant<csc::IHDR, csc::PLTE, csc::IDAT, csc::IEND>;
using v_sections = csc::vector<v_section>;

struct SUBSCRIBE {
  const csc::array<uint8_t, 8> data = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
  bool operator==(const csc::SUBSCRIBE& other) {
    return data == other.data;
  }
  bool operator!=(const csc::SUBSCRIBE& other) {
    return !(*this == other);
  }
};

} // namespace csc

export namespace csc {

struct f_construct {
  const csc::chunk& m_chunk;
  const csc::v_sections& m_common_deps;
  csc::inflater& m_stream;
  csc::vector<uint8_t>& m_image_data;
  f_construct(const csc::chunk& ch, const csc::v_sections& cd, csc::inflater& sm, csc::vector<uint8_t>& img)
      : m_chunk(ch), m_common_deps(cd), m_stream(sm), m_image_data(img) {
  }
  // статический полиморфизм
  csc::section_code_t operator()(csc::IHDR& b) {
    return b.construct(m_chunk);
  }
  csc::section_code_t operator()(csc::PLTE& b) {
    const csc::IHDR& header = csc::get<csc::IHDR>(m_common_deps[0]);
    return b.construct(m_chunk, header);
  }
  csc::section_code_t operator()(csc::IDAT& b) {
    const csc::IHDR& header = csc::get<csc::IHDR>(m_common_deps[0]);
    return b.construct(m_chunk, header, m_stream, m_image_data);
  }
  csc::section_code_t operator()(csc::IEND& b) {
    return b.construct(m_chunk);
  }
};

} // namespace csc
