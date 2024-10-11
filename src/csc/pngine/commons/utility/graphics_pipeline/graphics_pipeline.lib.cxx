module;
#include <type_traits>
export module csc.pngine.commons.utility.graphics_pipeline;

import stl.vector;
import stl.string;

import vulkan_hpp;

export namespace csc {
namespace pngine {

struct shader_stage {
  std::string entry_point;
  vk::ShaderModule bind_module;
  vk::ShaderStageFlagBits stage_type;
};

struct graphics_pipeline_config {
  std::vector<pngine::shader_stage> selected_stages{};

  std::vector<vk::VertexInputBindingDescription> vertex_input_bindings{};
  std::vector<vk::VertexInputAttributeDescription> vertex_input_attributes{};

  vk::PrimitiveTopology input_assembler_topology{};

  vk::Extent2D viewport_area{};
  float viewport_x{}, viewport_y{};

  vk::Extent2D scissors_area{};
  float scissors_x{}, scissors_y{};

  vk::PolygonMode rasterizer_poly_mode{};

  std::vector<vk::DynamicState> dynamic_states{};
};

template <typename T>
concept c_graphics_pipeline_config = std::is_same_v<pngine::graphics_pipeline_config, std::remove_reference_t<T>>;

} // namespace pngine
} // namespace csc
