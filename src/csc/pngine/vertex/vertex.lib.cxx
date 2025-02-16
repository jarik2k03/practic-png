module;
#include <glm/glm.hpp>
export module csc.pngine.vertex;

import vulkan_hpp;

import stl.array;
export namespace csc {
namespace pngine {

using vertex_attrubute_descriptions = std::array<vk::VertexInputAttributeDescription, 3u>;
struct vertex {
  glm::vec2 position;
  glm::vec3 vtx_color;
  glm::vec2 texcoord;

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
    /* vtx_color */
    descs[1].binding = 0u;
    descs[1].location = 1u;
    descs[1].format = vk::Format::eR32G32B32Sfloat;
    descs[1].offset = offsetof(vertex, vtx_color);
    /* texcoord */
    descs[2].binding = 0u;
    descs[2].location = 2u;
    descs[2].format = vk::Format::eR32G32Sfloat;
    descs[2].offset = offsetof(vertex, texcoord);
    return descs;
  }
};

struct MVP {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

// на самом деле здесь матрица 3х3, но шейдер не читает по 3 байта, поэтому выравнивает каждую строку
struct conversion {
  glm::mat4x3 image_colorspace;
  glm::mat4x3 monitor_colorspace;
};

} // namespace pngine
} // namespace csc
