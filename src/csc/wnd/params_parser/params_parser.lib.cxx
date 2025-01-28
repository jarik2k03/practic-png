module;
#include <utility>
#include <cstdint>
#include <cmath>
#include <bits/stl_algo.h>
export module csc.wnd.params_parser;

import stl.string;
import stl.stringstream;
import stl.iostream;

import glm_hpp;

export namespace csc {
namespace wnd {

using clip_params = std::tuple<int32_t, int32_t, uint32_t, uint32_t>;
using scale_params = std::tuple<float, float>;
using rotate_params = std::tuple<float>;

class params_parser {
 private:
  std::stringstream m_input;

 public:
  params_parser(const std::string& unparsed) : m_input(unparsed) {
  }

  wnd::clip_params parse_clip_params();
  wnd::scale_params parse_scale_params();
  wnd::rotate_params parse_rotate_params();
};

wnd::clip_params params_parser::parse_clip_params() {
  float beg_x = 0.f, beg_y = 0.f, end_x = 1.f, end_y = 1.f;
  m_input >> beg_x, m_input >> beg_y, m_input >> end_x, m_input >> end_y;
  const auto bx = static_cast<int32_t>(std::abs(std::floorf(beg_x))),
             by = static_cast<int32_t>(std::abs(std::floorf(beg_y)));
  const auto ex = static_cast<uint32_t>(std::abs(std::floorf(end_x))),
             ey = static_cast<uint32_t>(std::abs(std::floorf(end_y)));
  return {bx, by, ex, ey};
}

wnd::scale_params params_parser::parse_scale_params() {
  float scale_x = 1.f, scale_y = 1.f;
  m_input >> scale_x, m_input >> scale_y;
  scale_x = std::abs(scale_x), scale_y = std::abs(scale_y);

  scale_x = std::clamp(scale_x, 0.05f, std::numeric_limits<float>::max());
  scale_y = std::clamp(scale_y, 0.05f, std::numeric_limits<float>::max());

  return {scale_x, scale_y};
}

wnd::rotate_params params_parser::parse_rotate_params() {
  float rotate_degree = 0.f;
  m_input >> rotate_degree;
  const float rotate_radians = rotate_degree * glm::pi<float>() / 180;
  return {rotate_radians};
}

} // namespace wnd
} // namespace csc
