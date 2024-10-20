module;
#include <cstdint>
export module csc.png.picture.sections.PLTE;

export import stl.vector;
export import csc.png.commons.utility.pixel_formats;

export namespace csc {
namespace png {

struct PLTE {
  std::vector<rgb8> full_palette;
};

} // namespace png
} // namespace csc
