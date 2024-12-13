module;
#include <cstdint>
module csc.pngine.image_manipulator:shaderdata;
export import glm_hpp;

export namespace csc {
namespace pngine {

namespace clipping {
struct params {
  alignas(16) glm::uvec2 clip_offset;
  alignas(16) glm::uvec2 clip_size;
};
} // namespace clipping

} // namespace pngine
} // namespace csc
