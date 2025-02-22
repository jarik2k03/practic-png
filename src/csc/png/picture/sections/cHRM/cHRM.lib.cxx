module;
#include <cstdint>
export module csc.png.picture.sections.cHRM;

export namespace csc {
namespace png {

struct cHRM {
  uint32_t white_x, white_y;
  uint32_t red_x, red_y;
  uint32_t green_x, green_y;
  uint32_t blue_x, blue_y;
};

consteval png::cHRM generate_sRGB_D65() noexcept {
  csc::png::cHRM colorspace;
  colorspace.white_x = 31270u, colorspace.white_y = 32900u;
  colorspace.red_x = 64000u, colorspace.red_y = 33000u;
  colorspace.green_x = 30000u, colorspace.green_y = 60000u;
  colorspace.blue_x = 15000u, colorspace.blue_y = 6000u;
  return colorspace;
}

} // namespace png
} // namespace csc
