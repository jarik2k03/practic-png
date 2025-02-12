module;
#include <bits/stl_iterator.h>
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.graphics_pipeline;

import vulkan_hpp;

import csc.pngine.commons.utility.graphics_pipeline;
import stl.vector;
import stl.array;

export namespace csc {
namespace pngine {
class graphics_pipeline {
 private:
  const vk::Device* m_keep_device = nullptr;
  vk::Rect2D m_scissor{};
  vk::Viewport m_viewport{};
  vk::PipelineColorBlendAttachmentState m_color_blend_attachment{};
  pngine::graphics_pipeline_config m_config{};
  vk::Pipeline m_graphics_pipeline{};
  vk::Bool32 m_is_created;

 public:
  template <pngine::c_graphics_pipeline_config Config>
  explicit graphics_pipeline(
      const vk::Device& device,
      const vk::RenderPass& pass,
      const vk::PipelineLayout& layout,
      Config&& config);
  ~graphics_pipeline() noexcept;
  graphics_pipeline(graphics_pipeline&& move) noexcept;
  graphics_pipeline& operator=(graphics_pipeline&& move) noexcept;
  void clear() noexcept;
  vk::Pipeline get() const;
};

graphics_pipeline::graphics_pipeline(graphics_pipeline&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_scissor(move.m_scissor),
      m_viewport(move.m_viewport),
      m_color_blend_attachment(move.m_color_blend_attachment),
      m_config(std::move(move.m_config)),
      m_graphics_pipeline(move.m_graphics_pipeline),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
graphics_pipeline& graphics_pipeline::operator=(graphics_pipeline&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_graphics_pipeline = move.m_graphics_pipeline;
  m_keep_device = move.m_keep_device;
  m_scissor = move.m_scissor;
  m_viewport = move.m_viewport;
  m_color_blend_attachment = move.m_color_blend_attachment;
  m_config = std::move(move.m_config);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

template <pngine::c_graphics_pipeline_config Config>
graphics_pipeline::graphics_pipeline(
    const vk::Device& device,
    const vk::RenderPass& pass,
    const vk::PipelineLayout& layout,
    Config&& config)
    : m_keep_device(&device), m_config(std::forward<Config>(config)), m_is_created(false) {
  std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_infos;
  shader_stage_infos.reserve(m_config.selected_stages.size());
  for (const auto& stage : m_config.selected_stages) {
    vk::PipelineShaderStageCreateInfo stage_info{};
    stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    stage_info.stage = stage.stage_type;
    stage_info.module = stage.bind_module;
    stage_info.pName = stage.entry_point.data();
    shader_stage_infos.emplace_back(std::move(stage_info));
  }

  vk::PipelineVertexInputStateCreateInfo vertex_input_desc{};
  vertex_input_desc.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
  vertex_input_desc.pVertexBindingDescriptions = m_config.vtx_bindings.data();
  vertex_input_desc.vertexBindingDescriptionCount = m_config.vtx_bindings.size();

  vertex_input_desc.pVertexAttributeDescriptions = m_config.vtx_attributes.data();
  vertex_input_desc.vertexAttributeDescriptionCount = m_config.vtx_attributes.size();

  vk::PipelineInputAssemblyStateCreateInfo input_assembler_desc{};
  input_assembler_desc.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
  input_assembler_desc.topology = m_config.input_assembler_topology;
  input_assembler_desc.primitiveRestartEnable = vk::False;

  m_viewport.x = m_config.viewport_x, m_viewport.y = m_config.viewport_y;
  m_viewport.width = m_config.viewport_area.width, m_viewport.height = m_config.viewport_area.height;
  m_viewport.minDepth = 0.0f, m_viewport.maxDepth = 1.0f;

  m_scissor.offset = vk::Offset2D{static_cast<int32_t>(m_config.scissors_x), static_cast<int32_t>(m_config.scissors_y)};
  m_scissor.extent = m_config.scissors_area;

  vk::PipelineViewportStateCreateInfo viewport_state_desc{};
  viewport_state_desc.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
  viewport_state_desc.viewportCount = 1u;
  viewport_state_desc.pViewports = &m_viewport;
  viewport_state_desc.scissorCount = 1u;
  viewport_state_desc.pScissors = &m_scissor;

  vk::PipelineRasterizationStateCreateInfo rasterizer_desc{};
  rasterizer_desc.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
  rasterizer_desc.depthClampEnable = vk::False;
  rasterizer_desc.rasterizerDiscardEnable = vk::False;
  rasterizer_desc.polygonMode = m_config.rasterizer_poly_mode;
  rasterizer_desc.lineWidth = 1.0f;
  rasterizer_desc.cullMode = vk::CullModeFlagBits::eBack;
  rasterizer_desc.frontFace = vk::FrontFace::eCounterClockwise;
  rasterizer_desc.depthBiasEnable = vk::False;
  rasterizer_desc.depthBiasConstantFactor = 0.0f;
  rasterizer_desc.depthBiasClamp = 0.0f;
  rasterizer_desc.depthBiasSlopeFactor = 0.0f;

  vk::PipelineMultisampleStateCreateInfo msaa_state_desc{};
  msaa_state_desc.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
  msaa_state_desc.sampleShadingEnable = vk::False;
  msaa_state_desc.rasterizationSamples = vk::SampleCountFlagBits::e1;
  using enum vk::ColorComponentFlagBits;
  m_color_blend_attachment.colorWriteMask = eR | eG | eB | eA;
  m_color_blend_attachment.blendEnable = vk::True;

  m_color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
  m_color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
  m_color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;

  m_color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;
  m_color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
  m_color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;

  vk::PipelineColorBlendStateCreateInfo color_blend_desc{};
  color_blend_desc.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
  color_blend_desc.logicOpEnable = vk::False;
  color_blend_desc.logicOp = vk::LogicOp::eCopy;
  color_blend_desc.pAttachments = &m_color_blend_attachment;
  color_blend_desc.attachmentCount = 1u;
  color_blend_desc.blendConstants = std::array<float, 4>{0.f, 0.f, 0.f, 0.f};

  vk::PipelineDynamicStateCreateInfo dynamic_state_desc{};
  dynamic_state_desc.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
  dynamic_state_desc.pDynamicStates = m_config.dynamic_states.data();
  dynamic_state_desc.dynamicStateCount = m_config.dynamic_states.size();

  vk::GraphicsPipelineCreateInfo description{};
  description.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
  description.pStages = shader_stage_infos.data();
  description.stageCount = shader_stage_infos.size();
  description.pVertexInputState = &vertex_input_desc;
  description.pInputAssemblyState = &input_assembler_desc;
  description.pViewportState = &viewport_state_desc;
  description.pRasterizationState = &rasterizer_desc;
  description.pMultisampleState = &msaa_state_desc;
  description.pColorBlendState = &color_blend_desc;
  description.pDynamicState = &dynamic_state_desc;
  description.layout = layout; // выбранный макет
  description.renderPass = pass; // выбранный проход
  description.subpass = 0u;
  description.basePipelineHandle = vk::Pipeline{};
  m_graphics_pipeline = m_keep_device->createGraphicsPipeline(vk::PipelineCache(), description, nullptr).value;
  m_is_created = true;
}

vk::Pipeline graphics_pipeline::get() const {
  return m_graphics_pipeline;
}

void graphics_pipeline::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyPipeline(m_graphics_pipeline, nullptr);
    m_is_created = false;
  }
}

graphics_pipeline::~graphics_pipeline() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
