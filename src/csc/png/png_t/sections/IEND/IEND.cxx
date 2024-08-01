module;
export module csc.png.png_t.sections.IEND;

import :impl;

namespace csc {
export class IEND : private IEND_impl {
 public:
  IEND() : IEND_impl() {
  }
  ~IEND() = default;
  csc::section_code_t construct(const csc::chunk& raw) noexcept {
    return do_construct(raw);
  }
};

} // namespace csc
