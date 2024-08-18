module;
#include <cstdint>
export module csc.png.png_t.sections.inflater;

import :impl;

export namespace csc {


class inflater : private inflater_impl {
 public:
  inflater(const csc::vector<uint8_t>& in) : inflater_impl(in) {
  }
  ~inflater() = default;

  void inflate() {
    return this->do_inflate();
  }
};

} // namespace csc
