module;
#include <cstdint>
export module csc.png.png_t.sections;

import csc.stl_wrap.vector;
import csc.stl_wrap.array;
import csc.stl_wrap.variant;

export import csc.png.png_t.sections.chunk;
export import csc.png.png_t.sections.utils;

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
  f_construct(const csc::chunk& ch, const csc::v_sections& cd) : m_chunk(ch), m_common_deps(cd) {
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
    return b.construct(m_chunk, header);
  }
  csc::section_code_t operator()(csc::IEND& b) {
    return b.construct(m_chunk);
  }
};

} // namespace csc
