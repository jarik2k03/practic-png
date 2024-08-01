module;
#include <cstdint>
export module csc.png.png_t.sections.inflater;

import :impl;

export namespace csc {

constexpr unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024u;
}

class inflater : private inflater_impl {
 public:
  inflater(const csc::vector<uint8_t>& in) : inflater_impl(in) {
  }
  ~inflater() = default;

  void inflate(int flush) {
    return this->do_inflate(flush);
  }
};

} // namespace csc
