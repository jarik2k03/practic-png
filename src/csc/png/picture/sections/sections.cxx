module;
#include <cstdint>
export module csc.png.picture.sections;

import stl.vector;
import stl.variant;

export import csc.png.picture.sections.IHDR;
export import csc.png.picture.sections.PLTE;
export import csc.png.picture.sections.IEND;

export import csc.png.picture.sections.bKGD;
export import csc.png.picture.sections.tIME;
export import csc.png.picture.sections.cHRM;
export import csc.png.picture.sections.gAMA;
export import csc.png.picture.sections.hIST;
export import csc.png.picture.sections.pHYs;
export import csc.png.picture.sections.tRNS;
export namespace csc {
namespace png {

using v_section = std::variant<
    png::IHDR,
    png::PLTE,
    png::IEND,
    png::bKGD,
    png::tIME,
    png::cHRM,
    png::gAMA,
    png::hIST,
    png::pHYs,
    png::tRNS>;
using v_sections = std::vector<v_section>;

} // namespace png
} // namespace csc
