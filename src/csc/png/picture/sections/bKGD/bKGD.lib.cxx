module;
#include <cstdint>
export module csc.png.picture.sections.bKGD;

export import :attributes;

export namespace csc {

struct bKGD {
  csc::v_pixel_view color;
  csc::e_pixel_view_id color_type;
};

}

