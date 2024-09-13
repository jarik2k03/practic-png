module;
#include <cstdint>
export module csc.png.picture.sections.cHRM;

export namespace csc {

struct cHRM {
  uint32_t white_x, white_y;
  uint32_t red_x, red_y;
  uint32_t green_x, green_y;
  uint32_t blue_x, blue_y;
};

} // namespace csc
