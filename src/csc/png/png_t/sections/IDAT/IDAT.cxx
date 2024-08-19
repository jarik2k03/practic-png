module;
#include <cstdint>
export module csc.png.png_t.sections.IDAT;

import :impl;

import csc.stl_wrap.vector;
export namespace csc {

class IDAT : private IDAT_impl {
 public:
  IDAT() = default;
  ~IDAT() = default;
  csc::section_code_t construct(const csc::chunk& raw, const csc::IHDR& header, csc::inflater& infstream, csc::vector<uint8_t>& image_data) noexcept {
    return this->do_construct(raw, header, infstream, image_data);
  }
};

} // namespace csc
