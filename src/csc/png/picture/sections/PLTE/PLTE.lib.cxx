module;
#include <cstdint>
export module csc.png.picture.sections.PLTE;

export import cstd.stl_wrap.vector;
export import csc.png.commons.utility.pixel_formats;

export namespace csc {

struct PLTE {
  cstd::vector<rgb8> full_palette;
};

} // namespace csc
