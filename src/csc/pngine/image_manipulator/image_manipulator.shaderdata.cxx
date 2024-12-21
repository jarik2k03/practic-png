module;
#include <cstdint>
module csc.pngine.image_manipulator:shaderdata;
export import glm_hpp;

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

}


} // namespace pngine
} // namespace csc
