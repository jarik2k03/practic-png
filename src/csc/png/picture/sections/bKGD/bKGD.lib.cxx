module;
#include <cstdint>
export module csc.png.picture.sections.bKGD;

export import :attributes;

export namespace csc {
namespace png {

struct bKGD {
  png::v_pixel_view color;
  png::e_pixel_view_id color_type;
};

} // namespace png
} // namespace csc
