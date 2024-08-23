module;
#include <cstdint>
export module csc.png.deserializer.consume_chunk;

import :overloads;

export namespace csc {

class f_consume_chunk {
 private:
  const csc::chunk& m_chunk;
  const csc::v_sections& m_common_deps;
  csc::inflater& m_stream;
  csc::vector<uint8_t>& m_image_data;

 public:
  f_consume_chunk(const csc::chunk& ch, const csc::v_sections& cd, csc::inflater& sm, csc::vector<uint8_t>& img)
      : m_chunk(ch), m_common_deps(cd), m_stream(sm), m_image_data(img) {
  }
  // статический полиморфизм
  auto operator()(csc::IHDR& b) {
    return csc::consume_chunk(b, m_chunk);
  }
  auto operator()(csc::PLTE& b) {
    const csc::IHDR& header = csc::get<csc::IHDR>(m_common_deps[0]);
    return csc::consume_chunk(b, m_chunk, header);
  }
  auto operator()(csc::IDAT& b) {
    const csc::IHDR& header = csc::get<csc::IHDR>(m_common_deps[0]);
    return csc::consume_chunk(b, m_chunk, header, m_stream, m_image_data);
  }
  auto operator()(csc::IEND& b) {
    return csc::consume_chunk(b, m_chunk);
  }
};

} // namespace csc
