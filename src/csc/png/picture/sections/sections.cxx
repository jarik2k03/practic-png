module;
#include <cstdint>
export module csc.png.picture.sections;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.variant;

export import csc.png.picture.sections.IHDR;
export import csc.png.picture.sections.PLTE;
export import csc.png.picture.sections.IDAT;
export import csc.png.picture.sections.IEND;

export namespace csc {

using v_section = cstd::variant<csc::IHDR, csc::PLTE, csc::IDAT, csc::IEND>;
using v_sections = cstd::vector<v_section>;

} // namespace csc