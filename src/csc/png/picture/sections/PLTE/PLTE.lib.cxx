module;
#include <cstdint>
export module csc.png.picture.sections.PLTE;

export import stl.stl_wrap.vector;
export import csc.png.commons.utility.pixel_formats;

export namespace csc {

struct PLTE {
  std::vector<rgb8> full_palette;
};

} // namespace csc
