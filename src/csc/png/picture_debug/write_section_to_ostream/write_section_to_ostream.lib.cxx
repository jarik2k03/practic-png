module;
#include <cstdint>
export module csc.png.picture_debug.write_section_to_ostream;

import :overloads;

import csc.png.picture;

import stl.stl_wrap.ostream;

export namespace csc {

class f_write_section_to_ostream {
 private:
  std::ostream& m_debug;
  uint8_t m_bit_depth;

 public:
  f_write_section_to_ostream(std::ostream& os, uint8_t bd) : m_debug(os), m_bit_depth(bd) {
  }
  // статический полиморфизм
  void operator()(const csc::IHDR& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::PLTE& b) {
    csc::write_section_to_ostream(b, m_debug, m_bit_depth);
  }
  void operator()(const csc::IEND& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::bKGD& b) {
    csc::write_section_to_ostream(b, m_debug, m_bit_depth);
  }
  void operator()(const csc::tIME& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::cHRM& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::gAMA& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::hIST& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::pHYs& b) {
    csc::write_section_to_ostream(b, m_debug);
  }
  void operator()(const csc::tRNS& b) {
    csc::write_section_to_ostream(b, m_debug, m_bit_depth);
  }
};

} // namespace csc
