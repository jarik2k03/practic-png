module;
#include <cstdint>
export module csc.png.picture.sections;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.variant;

export import csc.png.picture.sections.IHDR;
export import csc.png.picture.sections.PLTE;
export import csc.png.picture.sections.IDAT;
export import csc.png.picture.sections.IEND;

export import csc.png.picture.sections.bKGD;
export import csc.png.picture.sections.tIME;
export import csc.png.picture.sections.cHRM;
export import csc.png.picture.sections.gAMA;
export import csc.png.picture.sections.hIST;
export import csc.png.picture.sections.pHYs;
export namespace csc {

struct dummy {};
using v_section = cstd::variant<
    csc::IHDR,
    csc::PLTE,
    csc::IDAT,
    csc::IEND,
    csc::bKGD,
    csc::tIME,
    csc::cHRM,
    csc::gAMA,
    csc::hIST,
    csc::pHYs,
    csc::dummy>;
using v_sections = cstd::vector<v_section>;

} // namespace csc
