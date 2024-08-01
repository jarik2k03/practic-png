module;
export module csc.png.png_t.sections.IDAT;

import :impl;

export namespace csc {

class IDAT : private IDAT_impl {
 public:
  IDAT() = default;
  ~IDAT() = default;
  csc::section_code_t construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
    return this->do_construct(raw, header);
  }
};

} // namespace csc
