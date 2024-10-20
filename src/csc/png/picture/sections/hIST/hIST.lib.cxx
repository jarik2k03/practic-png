module;
#include <cstdint>
export module csc.png.picture.sections.hIST;

export import stl.vector;
export import :attributes;
export namespace csc {
namespace png {

struct hIST {
  std::vector<png::color_frequency> histogram;
};

} // namespace png
} // namespace csc
