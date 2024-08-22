module;
export module csc.png.png_t.sections.PLTE;

import :impl;
export import :attributes;

namespace csc {

export class PLTE : private PLTE_impl {
 public:
  PLTE() : PLTE_impl() {
  }
  ~PLTE() = default;
  csc::section_code_t construct(const csc::chunk& raw, const csc::IHDR& header) noexcept {
    return do_construct(raw, header);
  }
  auto full_palette() const {
    return do_full_palette();
  }
};

} // namespace csc
