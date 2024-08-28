module;
#include <cstdint>
export module csc.png.picture.sections.hIST;

export import cstd.stl_wrap.vector;
export import :attributes;
export namespace csc {

struct hIST {
  cstd::vector<csc::color_frequency> histogram;
};

} // namespace csc
