module;
#include <cstdint>
export module csc.png.picture_debug.write_section_to_ostream;

import :overloads;

import csc.png.picture;

import stl.ostream;

export namespace csc {
namespace png {

class f_write_section_to_ostream {
 private:
  std::ostream& m_debug;
  uint8_t m_bit_depth;

 public:
  f_write_section_to_ostream(std::ostream& os, uint8_t bd) : m_debug(os), m_bit_depth(bd) {
  }
  // статический полиморфизм
  void operator()(const png::IHDR& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::PLTE& b) {
    png::write_section_to_ostream(b, m_debug, m_bit_depth);
  }
  void operator()(const png::IEND& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::bKGD& b) {
    png::write_section_to_ostream(b, m_debug, m_bit_depth);
  }
  void operator()(const png::tIME& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::cHRM& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::gAMA& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::hIST& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::pHYs& b) {
    png::write_section_to_ostream(b, m_debug);
  }
  void operator()(const png::tRNS& b) {
    png::write_section_to_ostream(b, m_debug, m_bit_depth);
  }
};

} // namespace png
} // namespace csc
