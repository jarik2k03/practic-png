module;
#include <cstdint>
export module csc.png.png_t.sections.IHDR;

import :impl;
export import :attributes;
import csc.png.png_t.sections.chunk;
import csc.png.png_t.sections.utils;

namespace csc {
export class IHDR : private IHDR_impl {
 public:
  IHDR() : IHDR_impl() {
  }
  ~IHDR() = default;
  uint32_t width() const noexcept {
    return do_width();
  }
  uint32_t height() const noexcept {
    return do_height();
  }
  uint8_t bit_depth() const noexcept {
    return do_bit_depth();
  }
  csc::color_type_t color_type() const noexcept {
    return do_color_type();
  }
  csc::compression_t compression() const noexcept {
    return do_compression();
  }
  csc::filter_t filter() const noexcept {
    return do_filter();
  }
  csc::interlace_t interlace() const noexcept {
    return do_interlace();
  }
  uint32_t crc_adler() const noexcept {
    return do_crc_adler();
  }
  csc::section_code_t construct(const csc::chunk& raw) noexcept {
    return do_construct(raw);
  }
};

} // namespace csc
