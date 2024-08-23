module;
#include <cstdint>
export module csc.png.picture.sections;

import csc.stl_wrap.vector;
import csc.stl_wrap.variant;

export import csc.png.picture.sections.IHDR;
export import csc.png.picture.sections.PLTE;
export import csc.png.picture.sections.IDAT;
export import csc.png.picture.sections.IEND;

export namespace csc {

using v_section = csc::variant<csc::IHDR, csc::PLTE, csc::IDAT, csc::IEND>;
using v_sections = csc::vector<v_section>;

} // namespace csc
