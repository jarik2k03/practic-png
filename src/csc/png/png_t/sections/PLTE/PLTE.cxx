module;
export module csc.png.png_t.sections.PLTE;

import :impl;

namespace csc {

export class PLTE : private PLTE_impl {
 public:
  PLTE() : PLTE_impl() {
  }
  ~PLTE() = default;
  csc::section_code_t construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
    return do_construct(raw, header);
  }
};

} // namespace csc
