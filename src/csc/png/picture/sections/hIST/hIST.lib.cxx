module;
#include <cstdint>
export module csc.png.picture.sections.hIST;

export import stl.vector;
export import :attributes;
export namespace csc {

struct hIST {
  std::vector<csc::color_frequency> histogram;
};

} // namespace csc
