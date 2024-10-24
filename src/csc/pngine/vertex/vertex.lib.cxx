module;
#include <glm/glm.hpp>
export module csc.pngine.vertex;

import vulkan_hpp;

import stl.array;
export namespace csc {
namespace pngine {

using vertex_attrubute_descriptions = std::array<vk::VertexInputAttributeDescription, 2u>;
struct vertex {
  glm::vec2 position;
  glm::vec3 vtx_color;

  static auto get_binding_description() {
    vk::VertexInputBindingDescription desc{};
    desc.binding = 0u;
    desc.stride = sizeof(vertex);
    desc.inputRate = vk::VertexInputRate::eVertex;
    return desc;
  }
  static auto get_attribute_descriptions() {
    vertex_attrubute_descriptions descs{};
    /* position */
    descs[0].binding = 0u;
    descs[0].location = 0u;
    descs[0].format = vk::Format::eR32G32Sfloat;
    descs[0].offset = offsetof(vertex, position);
    /* color */
    descs[1].binding = 0u;
    descs[1].location = 1u;
    descs[1].format = vk::Format::eR32G32B32Sfloat;
    descs[1].offset = offsetof(vertex, vtx_color);
    return descs;
  }
};

} // namespace pngine
} // namespace csc
