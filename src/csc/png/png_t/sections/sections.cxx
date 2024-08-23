module;
#include <cstdint>
export module csc.png.png_t.sections;

import csc.stl_wrap.vector;
import csc.stl_wrap.variant;

export import csc.png.png_t.sections.IHDR;
export import csc.png.png_t.sections.PLTE;
export import csc.png.png_t.sections.IDAT;
export import csc.png.png_t.sections.IEND;

export namespace csc {

using v_section = csc::variant<csc::IHDR, csc::PLTE, csc::IDAT, csc::IEND>;
using v_sections = csc::vector<v_section>;

} // namespace csc
