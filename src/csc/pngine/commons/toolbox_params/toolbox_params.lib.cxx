module;
#include <cstdint>
export module csc.pngine.commons.toolbox_params;

import glm_hpp;

export namespace csc {
namespace pngine {

namespace clipping {
struct params {
  alignas(16) glm::ivec2 clip_offset;
};
} // namespace clipping

namespace scaling {
struct params {
  alignas(16) glm::vec2 scale_coeff;
};
} // namespace scaling

namespace rotating {
struct params {
  glm::vec2 in_middle_idx;
  glm::vec2 out_middle_idx;
  float cos_angle;
  float sin_angle;
};
} // namespace rotating

}
}
