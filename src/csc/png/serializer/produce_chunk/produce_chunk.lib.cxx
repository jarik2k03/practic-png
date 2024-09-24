module;
#include <cstdint>
export module csc.png.serializer.produce_chunk;

import :utility;
import :overloads;

export import csc.png.commons.chunk;

export namespace csc {
namespace png {

class f_produce_chunk {
 private:
  png::chunk& m_chunk;

 public:
  constexpr f_produce_chunk(png::chunk& ch) : m_chunk(ch) {
  }
  // статический полиморфизм
  auto operator()(const png::IHDR& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::PLTE& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::IEND& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::bKGD& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::tIME& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::cHRM& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::gAMA& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::hIST& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::pHYs& b) {
    return png::produce_chunk(b, m_chunk);
  }
  auto operator()(const png::tRNS& b) {
    return png::produce_chunk(b, m_chunk);
  }
};

} // namespace png
} // namespace csc
