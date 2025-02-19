module;
#include <cstdint>
#include <tuple>
#include <bits/stl_algobase.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algobase.h>
#include <bits/ranges_algo.h>
#include <cstring>
#include <cmath>
#include <shaders_triangle.h>
export module csc.pngine;

export import :attributes;

import stl.vector;
import stl.array;
import stl.string;
export import stl.string_view;
import stl.stdexcept;
#ifndef NDEBUG
import stl.iostream;
#endif
import csc.png.picture.sections.IHDR;
import csc.png.picture.sections.cHRM;

import csc.pngine.instance;
import csc.pngine.instance.device;
import csc.pngine.vertex;
export import csc.pngine.image_manipulator;

import csc.pngine.commons.utility.graphics_pipeline;

import vulkan_hpp;
import glm_hpp;

namespace csc {
namespace pngine {

const std::vector<pngine::vertex> basic_rectangle = {
    // меш изображения
    pngine::vertex{{-0.5f, -0.5f}, {1.f, 1.f, 1.f}, {1.f, 0.f}},
    pngine::vertex{{0.5f, -0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f}},
    pngine::vertex{{0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, 1.f}},
    pngine::vertex{{-0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f, 1.f}},
};
const std::vector<uint16_t> rectangle_ids = {0, 1, 2, 2, 3, 0};

export class pngine_core {
 private:
  std::string m_app_name{"Application"};
  pngine::version m_app_version = pngine::bring_version(1u, 0u, 0u);
  pngine::version m_vk_api_version = pngine::bring_version(1u, 3u, 256u);
  std::string m_gpu_name;

  png::IHDR* m_png_info = nullptr;

  pngine::instance m_instance;
  uint32_t m_current_frame = 0u;
  static constexpr uint32_t m_flight_count = 2u;

  vk::UniqueDescriptorSetLayout m_descr_layout_0;
  vk::UniqueDescriptorSetLayout m_descr_layout_1;

  vk::UniquePipelineLayout m_pipeline_layout;

  vk::UniqueRenderPass m_render_pass;

  /* аттачменты (в т.ч. конечное изображение из свопчейна) */
  std::vector<vk::UniqueImageView> m_swapchain_image_views;

  std::vector<vk::UniqueFramebuffer> m_framebuffers;

  std::array<vk::Semaphore, m_flight_count> m_image_available_s;
  std::array<vk::Semaphore, m_flight_count> m_render_finished_s;
  std::array<vk::Fence, m_flight_count> m_in_flight_f;

  vk::UniqueBuffer m_stage_png_surface_mesh;
  vk::UniqueDeviceMemory m_stage_png_surface_mesh_memory;
  vk::UniqueBuffer m_png_surface_mesh;
  vk::UniqueDeviceMemory m_png_surface_mesh_memory;

  vk::UniqueBuffer m_stage_png_surface_ids;
  vk::UniqueDeviceMemory m_stage_png_surface_ids_memory;
  vk::UniqueBuffer m_png_surface_ids;
  vk::UniqueDeviceMemory m_png_surface_ids_memory;

  std::array<vk::UniqueBuffer, m_flight_count> m_uniform_mvps;
  vk::UniqueBuffer m_uniform_conversion;

  void* m_uniform_mapping;
  vk::UniqueDeviceMemory m_uniforms_memory;

  vk::UniqueDescriptorPool m_descr_pool;
  std::vector<vk::UniqueDescriptorSet> m_drawing_descr_sets_0;
  std::vector<vk::UniqueDescriptorSet> m_menu_descr_sets_0;
  std::vector<vk::UniqueDescriptorSet> m_descr_sets_1;

  vk::UniqueSampler m_image_sampler{};

  vk::UniqueImage m_menu_image_buffer{};
  vk::UniqueImageView m_menu_image_view{};
  vk::UniqueDeviceMemory m_menu_image_memory{};

  vk::UniqueCommandPool m_graphics_pool;
  vk::UniqueCommandPool m_transfer_pool;
  std::vector<vk::CommandBuffer> m_command_buffers;

  pngine::image_manipulator m_toolbox;
  void Create_Swapchain_Views();
  void Create_Framebuffers();

  void Create_Sampler();
  void Transfer_Image_HTOD(vk::Image dst, vk::Buffer src, vk::Extent2D size);
  void Update_Descriptor_Set_0(
      const std::vector<vk::UniqueDescriptorSet>& dst_sets,
      uint32_t mvp_offset,
      vk::ImageView image);
  void Update_Descriptor_Set_1();

  void Update_Menu(uint32_t frame_index);
  void Update_Image(uint32_t frame_index);

 public:
  pngine_core() = delete;
  pngine_core(std::string app_name, pngine::version app_version, const std::vector<const char*>& surface_extensions);
  void setup_surface(vk::SurfaceKHR render_target);
  void setup_gpu_and_renderer(std::string gpu_name, vk::Extent2D render_target_size);

  void init_menu(const std::vector<uint8_t>& blob);
  void change_drawing(const std::vector<uint8_t>& blob, png::IHDR& img_info);
  void change_drawing(vk::ImageView drawing_view, png::IHDR& img_info);
  void recreate_swapchain(vk::Extent2D framebuffer_size);
  void apply_colorspace(const png::cHRM& img_info);
  void render_frame();
  void load_mesh();
  ~pngine_core() noexcept;

  vk::Instance get_instance() const noexcept;
  const pngine::image_manipulator& get_toolbox() const noexcept;
  pngine::image_manipulator& get_toolbox() noexcept;
  const char* get_engine_name() const noexcept;
  pngine::human_version get_engine_version() const noexcept;
  pngine::human_version get_vk_api_version() const noexcept;
  std::string_view get_app_name() const noexcept;
};
pngine_core::pngine_core(std::string nm, pngine::version ver, const std::vector<const char*>& surface_extensions)
    : m_app_name(std::move(nm)), m_app_version(ver) {
  vk::ApplicationInfo app_info(
      m_app_name.c_str(), m_app_version, pngine::bring_engine_name(), pngine::bring_engine_version(), m_vk_api_version);
  // высокоуровневый алгоритм...
  m_instance = pngine::instance(app_info, surface_extensions);
#ifndef NDEBUG
  m_instance.create_debug_reportEXT();
#endif
}
void pngine_core::setup_surface(vk::SurfaceKHR render_target) {
  m_instance.create_surfaceKHR(render_target);
}

void pngine_core::Create_Sampler() {
  /* image sampler */
  vk::SamplerCreateInfo sampler_info{};
  sampler_info.sType = vk::StructureType::eSamplerCreateInfo;
  // sampler_info.magFilter = vk::Filter::eLinear;
  sampler_info.minFilter = vk::Filter::eNearest;
  sampler_info.addressModeU = vk::SamplerAddressMode::eClampToBorder;
  sampler_info.addressModeV = vk::SamplerAddressMode::eClampToBorder;
  sampler_info.addressModeW = vk::SamplerAddressMode::eClampToBorder;
  sampler_info.anisotropyEnable = vk::False;
  sampler_info.maxAnisotropy = 1.f;
  sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
  sampler_info.unnormalizedCoordinates = vk::False;
  sampler_info.compareEnable = vk::False;
  sampler_info.compareOp = vk::CompareOp::eAlways;
  sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
  sampler_info.mipLodBias = 0.f, sampler_info.minLod = 0.f, sampler_info.maxLod = 0.f;

  m_image_sampler = m_instance.get_device().get().createSamplerUnique(sampler_info, nullptr);
}

void pngine_core::Create_Swapchain_Views() {
  auto& device = m_instance.get_device();
  auto& swapchain = device.get_swapchain();
  /* создаем представления свопчейн изображений */
  std::vector<vk::UniqueImageView> views;
  views.reserve(swapchain.get_images().size());

  std::ranges::for_each(swapchain.get_images(), [&](const auto& img) {
    vk::ImageViewCreateInfo image_view_info{};
    image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
    image_view_info.image = img;
    image_view_info.format = swapchain.get_image_format();
    image_view_info.viewType = vk::ImageViewType::e2D;
    image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

    views.emplace_back(device.get().createImageViewUnique(image_view_info, nullptr));
  });

  m_swapchain_image_views = std::move(views);
}
void pngine_core::Create_Framebuffers() {
  auto& device = m_instance.get_device();
  auto& swapchain = device.get_swapchain();
  /* создаем фреймбуферы на основе свопчейна и рендер пасса */
  std::vector<vk::UniqueFramebuffer> fbs;
  m_framebuffers.clear(), m_framebuffers.reserve(m_swapchain_image_views.size());

  for (uint32_t index = 0u; index < m_swapchain_image_views.size(); ++index) {
    const auto attachments = std::array{m_swapchain_image_views[index].get()};

    vk::FramebufferCreateInfo fb_create_info{};
    fb_create_info.sType = vk::StructureType::eFramebufferCreateInfo;
    fb_create_info.renderPass = m_render_pass.get();
    fb_create_info.pAttachments = attachments.data();
    fb_create_info.attachmentCount = attachments.size();
    fb_create_info.width = swapchain.get_extent().width;
    fb_create_info.height = swapchain.get_extent().height;
    fb_create_info.layers = 1u;

    m_framebuffers.emplace_back(device.get().createFramebufferUnique(fb_create_info, nullptr));
  }
}

void pngine_core::setup_gpu_and_renderer(std::string gpu_name, vk::Extent2D render_target_size) {
  m_gpu_name = std::move(gpu_name);
  m_instance.bring_physical_devices();
  auto& device = m_instance.create_device(m_gpu_name);
  device.create_swapchainKHR(render_target_size, m_instance.get_swapchainKHR_dispatch());

  Create_Sampler();

  Create_Swapchain_Views();

  device.create_shader_module("vs_triangle", pngine::shaders::shaders_triangle::vert_path);
  device.create_shader_module("fs_triangle", pngine::shaders::shaders_triangle::frag_path);

  /* descriptor layouts */
  vk::DescriptorSetLayoutBinding u0_0_bind(
      0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr);
  vk::DescriptorSetLayoutBinding t1_0_bind(
      1u, vk::DescriptorType::eSampledImage, 1u, vk::ShaderStageFlagBits::eFragment, nullptr);
  const auto binds_0 = std::array{u0_0_bind, t1_0_bind}; // deduction guides!

  vk::DescriptorSetLayoutCreateInfo descriptor_layout_0_info{};
  descriptor_layout_0_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
  descriptor_layout_0_info.bindingCount = binds_0.size();
  descriptor_layout_0_info.pBindings = binds_0.data();

  m_descr_layout_0 = device.get().createDescriptorSetLayoutUnique(descriptor_layout_0_info, nullptr);

  vk::DescriptorSetLayoutBinding u0_1_bind(
      0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eFragment, nullptr);
  vk::DescriptorSetLayoutBinding s1_1_bind(
      1u, vk::DescriptorType::eSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr);
  const auto binds_1 = std::array{u0_1_bind, s1_1_bind}; // deduction guides!

  vk::DescriptorSetLayoutCreateInfo descriptor_layout_1_info{};
  descriptor_layout_1_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
  descriptor_layout_1_info.bindingCount = binds_1.size();
  descriptor_layout_1_info.pBindings = binds_1.data();

  m_descr_layout_1 = device.get().createDescriptorSetLayoutUnique(descriptor_layout_1_info, nullptr);

  const auto descr_layouts = std::array{m_descr_layout_0.get(), m_descr_layout_1.get()};
  vk::PipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  pipeline_layout_info.setLayoutCount = descr_layouts.size();
  pipeline_layout_info.pSetLayouts = descr_layouts.data();
  m_pipeline_layout = device.get().createPipelineLayoutUnique(pipeline_layout_info, nullptr);

  /* render pass */
  vk::AttachmentReference r_swapchain_output{};
  r_swapchain_output.attachment = 0u;
  r_swapchain_output.layout = vk::ImageLayout::eColorAttachmentOptimal;

  const auto out_color_references = std::array{r_swapchain_output};

  vk::SubpassDescription subpass{};
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.pColorAttachments = out_color_references.data();
  subpass.colorAttachmentCount = out_color_references.size();
  subpass.pInputAttachments = nullptr;
  subpass.inputAttachmentCount = 0u;

  const auto subpasses = std::array{subpass};

  // Описание назначения каждого аттачмента
  vk::AttachmentDescription present_desc = pngine::swapchain_image_preset(device.get_swapchain().get_image_format());

  const auto descriptions = std::array{present_desc};

  vk::RenderPassCreateInfo pass_info{};
  pass_info.sType = vk::StructureType::eRenderPassCreateInfo;
  pass_info.pAttachments = descriptions.data();
  pass_info.attachmentCount = descriptions.size();

  pass_info.pSubpasses = subpasses.data();
  pass_info.subpassCount = subpasses.size();

  m_render_pass = device.get().createRenderPassUnique(pass_info, nullptr);

  Create_Framebuffers();

  /* creating pipeline and framebuffers */
  pngine::graphics_pipeline_config triangle_cfg;
  const auto shader_stages = {
      pngine::shader_stage{"main", device.get_shader_module("vs_triangle").get(), vk::ShaderStageFlagBits::eVertex},
      pngine::shader_stage{"main", device.get_shader_module("fs_triangle").get(), vk::ShaderStageFlagBits::eFragment}};
  triangle_cfg.selected_stages = shader_stages;
  triangle_cfg.input_assembler_topology = vk::PrimitiveTopology::eTriangleList;
  triangle_cfg.viewport_area = device.get_swapchain().get_extent();
  triangle_cfg.scissors_area = device.get_swapchain().get_extent();
  triangle_cfg.rasterizer_poly_mode = vk::PolygonMode::eFill;
  const auto attr_descs = pngine::vertex::get_attribute_descriptions();
  const auto bind_descs = pngine::vertex::get_binding_description();
  triangle_cfg.vtx_bindings = {bind_descs};
  triangle_cfg.vtx_attributes = std::vector(attr_descs.cbegin(), attr_descs.cend());
  triangle_cfg.dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  device.create_pipeline("basic_pipeline", m_pipeline_layout, m_render_pass, triangle_cfg);
  /* vertex buffer */
  const uint32_t vtx_buf_size = sizeof(pngine::vertex) * basic_rectangle.size();
  std::tie(m_stage_png_surface_mesh, m_stage_png_surface_mesh_memory) = device.create_buffer(
      vtx_buf_size,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  void* data = device.get().mapMemory(
      m_stage_png_surface_mesh_memory.get(), 0u, basic_rectangle.size() * sizeof(pngine::vertex));
  ::memcpy(data, basic_rectangle.data(), vtx_buf_size);
  device.get().unmapMemory(m_stage_png_surface_mesh_memory.get());

  std::tie(m_png_surface_mesh, m_png_surface_mesh_memory) = device.create_buffer(
      vtx_buf_size,
      vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
      vk::MemoryPropertyFlagBits::eDeviceLocal);
  /* index buffer */
  const uint32_t idx_buf_size = sizeof(uint16_t) * rectangle_ids.size();
  std::tie(m_stage_png_surface_ids, m_stage_png_surface_ids_memory) = device.create_buffer(
      vtx_buf_size,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  data =
      device.get().mapMemory(m_stage_png_surface_ids_memory.get(), 0u, basic_rectangle.size() * sizeof(pngine::vertex));
  ::memcpy(data, rectangle_ids.data(), idx_buf_size);
  device.get().unmapMemory(m_stage_png_surface_ids_memory.get());

  std::tie(m_png_surface_ids, m_png_surface_ids_memory) = device.create_buffer(
      idx_buf_size,
      vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  /* uniform buffers */
  vk::BufferCreateInfo mvp_info{};
  mvp_info.sType = vk::StructureType::eBufferCreateInfo;
  mvp_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
  mvp_info.size = sizeof(pngine::MVP) * 2u;
  mvp_info.sharingMode = vk::SharingMode::eExclusive;

  vk::BufferCreateInfo cnv_info{};
  cnv_info.sType = vk::StructureType::eBufferCreateInfo;
  cnv_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
  cnv_info.size = sizeof(pngine::conversion);
  cnv_info.sharingMode = vk::SharingMode::eExclusive;

  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    m_uniform_mvps[i] = device.get().createBufferUnique(mvp_info, nullptr);
  }
  m_uniform_conversion = device.get().createBufferUnique(cnv_info, nullptr);

  const auto mvp_props = device.get().getBufferMemoryRequirements(m_uniform_mvps[0u].get());
  const auto cnv_props = device.get().getBufferMemoryRequirements(m_uniform_conversion.get());
  const auto supported_props = device.get_physdev().getMemoryProperties();
  using mp = vk::MemoryPropertyFlagBits;
  constexpr const auto required_props = mp::eHostVisible | mp::eHostCoherent;

  vk::MemoryAllocateInfo alloc_info{};
  alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
  alloc_info.allocationSize = mvp_props.size * m_flight_count + cnv_props.size; // аллоцируем под две юниформы
  alloc_info.memoryTypeIndex = pngine::choose_memory_type(supported_props, mvp_props, required_props);
  m_uniforms_memory = device.get().allocateMemoryUnique(alloc_info, nullptr);

  uint32_t mem_offset = 0u;
  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    device.get().bindBufferMemory(m_uniform_mvps[i].get(), m_uniforms_memory.get(), mem_offset);
    mem_offset += sizeof(pngine::MVP) * 2u;
  }
  device.get().bindBufferMemory(m_uniform_conversion.get(), m_uniforms_memory.get(), mem_offset);

  m_uniform_mapping = device.get().mapMemory(m_uniforms_memory.get(), 0u, alloc_info.allocationSize);

  /* creating pools and allocating buffers for render commands */
  m_graphics_pool = device.create_graphics_command_pool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
  m_transfer_pool = device.create_transfer_command_pool(vk::CommandPoolCreateFlagBits::eTransient);

  vk::CommandBufferAllocateInfo alloc_desc{};
  alloc_desc.sType = vk::StructureType::eCommandBufferAllocateInfo;
  alloc_desc.commandPool = m_graphics_pool.get();
  alloc_desc.level = vk::CommandBufferLevel::ePrimary;
  alloc_desc.commandBufferCount = m_flight_count;

  m_command_buffers = device.get().allocateCommandBuffers(alloc_desc);

  /* descriptor pool */
  vk::DescriptorPoolSize mvp_pool_size(vk::DescriptorType::eUniformBuffer, m_flight_count * 2u * 2u);
  vk::DescriptorPoolSize image_texture_pool_size(vk::DescriptorType::eSampledImage, m_flight_count * 2u);
  vk::DescriptorPoolSize image_sampler_pool_size(vk::DescriptorType::eSampler, m_flight_count);
  auto pool_sizes = std::array{mvp_pool_size, image_sampler_pool_size, image_texture_pool_size};

  vk::DescriptorPoolCreateInfo des_pool_info{};
  des_pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
  des_pool_info.pPoolSizes = pool_sizes.data();
  des_pool_info.poolSizeCount = pool_sizes.size();
  des_pool_info.maxSets = 32u; // прибито гвоздями
  des_pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

  m_descr_pool = device.get().createDescriptorPoolUnique(des_pool_info, nullptr);

  /* allocation of graphics_pipeline descriptors */
  auto layouts0 = std::array<vk::DescriptorSetLayout, m_flight_count * 2u>{};
  layouts0.fill(m_descr_layout_0.get());
  vk::DescriptorSetAllocateInfo alloc_sets_0_info{};
  alloc_sets_0_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
  alloc_sets_0_info.descriptorPool = m_descr_pool.get();
  alloc_sets_0_info.descriptorSetCount = layouts0.size();
  alloc_sets_0_info.pSetLayouts = layouts0.data();

  auto unsorted_sets = device.get().allocateDescriptorSetsUnique(alloc_sets_0_info);

  m_drawing_descr_sets_0.resize(m_flight_count);
  m_menu_descr_sets_0.resize(m_flight_count);

  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    m_drawing_descr_sets_0[i] = std::move(unsorted_sets[i]);
    m_menu_descr_sets_0[i] = std::move(unsorted_sets[i + m_flight_count]);
  } // группируем наборы дескрипторов

  const auto layouts1 = std::array{m_descr_layout_1.get(), m_descr_layout_1.get()};
  vk::DescriptorSetAllocateInfo alloc_sets_1_info{};
  alloc_sets_1_info.descriptorPool = m_descr_pool.get();
  alloc_sets_1_info.descriptorSetCount = layouts1.size();
  alloc_sets_1_info.pSetLayouts = layouts1.data();

  m_descr_sets_1 = device.get().allocateDescriptorSetsUnique(alloc_sets_1_info);

  /* syncronisation primitives */
  vk::SemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = vk::StructureType::eSemaphoreCreateInfo;

  vk::FenceCreateInfo fence_info{};
  fence_info.sType = vk::StructureType::eFenceCreateInfo;
  fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    m_render_finished_s[i] = device.get().createSemaphore(semaphore_info);
    m_image_available_s[i] = device.get().createSemaphore(semaphore_info);
    m_in_flight_f[i] = device.get().createFence(fence_info);
  }
}

pngine_core::~pngine_core() noexcept {
  auto& device = m_instance.get_device();
  device.get().waitIdle();

  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    device.get().destroySemaphore(m_render_finished_s[i]);
    device.get().destroySemaphore(m_image_available_s[i]);
    device.get().destroyFence(m_in_flight_f[i]);
  }
}

void pngine_core::recreate_swapchain(vk::Extent2D framebuffer_size) {
  auto& dev = m_instance.get_device();
  dev.get().waitIdle();
  dev.create_swapchainKHR(framebuffer_size, m_instance.get_swapchainKHR_dispatch()); // operator= очистит старый класс
  Create_Swapchain_Views(); // пересоздаст изобрвжения под новый свопчейн
  Create_Framebuffers(); // зависит от этих изображений
}

void pngine_core::change_drawing(vk::ImageView drawing_view, png::IHDR& img_info) {
  m_png_info = &img_info;
  Update_Descriptor_Set_0(m_drawing_descr_sets_0, 0u, drawing_view);
  Update_Descriptor_Set_1();
}

void pngine_core::change_drawing(const std::vector<uint8_t>& blob, png::IHDR& img_info) {
  auto& device = m_instance.get_device();
  m_png_info = &img_info;
  /* image data */
  const auto staged = device.create_buffer(
      blob.size(),
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  void* staged_image_mapping = device.get().mapMemory(staged.second.get(), 0u, blob.size());
  ::memcpy(staged_image_mapping, blob.data(), blob.size());
  device.get().unmapMemory(staged.second.get());

  /* clip & rotate & scale */
  m_toolbox = pngine::image_manipulator(
      device, staged.first, {m_png_info->width, m_png_info->height}); // выполняет преобразования изображений

  change_drawing(m_toolbox.get_first_image_view(), img_info);
}

void pngine_core::init_menu(const std::vector<uint8_t>& blob) {
  auto& device = m_instance.get_device();

  /* image data */
  const auto staged = device.create_buffer(
      blob.size(),
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  void* staged_image_mapping = device.get().mapMemory(staged.second.get(), 0u, blob.size());
  ::memcpy(staged_image_mapping, blob.data(), blob.size());
  device.get().unmapMemory(staged.second.get());

  /* image for render */
  std::tie(m_menu_image_buffer, m_menu_image_memory) = device.create_image(
      vk::Extent2D(64u, 256u),
      vk::Format::eR8G8B8A8Unorm,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  Transfer_Image_HTOD(m_menu_image_buffer.get(), staged.first.get(), vk::Extent2D(64u, 256u));

  /* image view */
  vk::ImageViewCreateInfo image_view_info{};
  image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  image_view_info.image = m_menu_image_buffer.get();
  image_view_info.format = vk::Format::eR8G8B8A8Unorm;
  image_view_info.viewType = vk::ImageViewType::e2D;
  image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  m_menu_image_view = device.get().createImageViewUnique(image_view_info, nullptr);
  Update_Descriptor_Set_0(m_menu_descr_sets_0, sizeof(pngine::MVP), m_menu_image_view.get());
}

void pngine_core::load_mesh() {
  const auto& device = m_instance.get_device();
  /* allocate transfer commands for copy image */
  vk::CommandBufferAllocateInfo alloc_desc{};
  alloc_desc.sType = vk::StructureType::eCommandBufferAllocateInfo;
  alloc_desc.commandPool = m_transfer_pool.get();
  alloc_desc.level = vk::CommandBufferLevel::ePrimary;
  alloc_desc.commandBufferCount = 1u;

  auto transfer_buffers = device.get().allocateCommandBuffers(alloc_desc);
  auto& transfer_buffer = transfer_buffers[0u];

  /* command buffer option */
  vk::CommandBufferBeginInfo begin_info{};
  begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  transfer_buffer.begin(begin_info);
  vk::BufferCopy copy_vtx_region(0u, 0u, sizeof(pngine::vertex) * basic_rectangle.size());
  transfer_buffer.copyBuffer(m_stage_png_surface_mesh.get(), m_png_surface_mesh.get(), 1u, &copy_vtx_region);
  vk::BufferCopy copy_idx_region(0u, 0u, sizeof(uint16_t) * rectangle_ids.size());
  transfer_buffer.copyBuffer(m_stage_png_surface_ids.get(), m_png_surface_ids.get(), 1u, &copy_idx_region);
  transfer_buffer.end();

  vk::SubmitInfo submit_info{};
  submit_info.sType = vk::StructureType::eSubmitInfo;
  submit_info.pCommandBuffers = &transfer_buffer;
  submit_info.commandBufferCount = 1u;
  auto transfer_queue = device.get_transfer_queue();
  const vk::Result r = transfer_queue.submit(1u, &submit_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось записать вершинный буфер в очередь передачи!");
  transfer_queue.waitIdle();
  device.get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);
}
void pngine_core::apply_colorspace(const png::cHRM& img_info) {
  constexpr const auto d = 100'000.f; // преобразование данных чанка в float (div 100'000)
  const float xr = static_cast<float>(img_info.red_x) / d, yr = static_cast<float>(img_info.red_y) / d;
  const float xg = static_cast<float>(img_info.green_x) / d, yg = static_cast<float>(img_info.green_y) / d;
  const float xb = static_cast<float>(img_info.blue_x) / d, yb = static_cast<float>(img_info.blue_y) / d;
  const float xw = static_cast<float>(img_info.white_x) / d, yw = static_cast<float>(img_info.white_y) / d;

  const float Xr = xr / yr, Yr = 1.f, Zr = (1.f - xr - yr) / yr;
  const float Xg = xg / yg, Yg = 1.f, Zg = (1.f - xg - yg) / yg;
  const float Xb = xb / yb, Yb = 1.f, Zb = (1.f - xb - yb) / yb;
  const float Xw = xw / yw, Yw = 1.f, Zw = (1.f - xw - yw) / yw;

  const auto XYZ = glm::mat3x3(
      Xr, Xg, Xb,
      Yr, Yg, Yb,
      Zr, Zg, Zb);
  const auto XYZ_inverted = glm::inverse(glm::transpose(XYZ));
  const auto S = XYZ_inverted * glm::vec3(Xw, Yw, Zw);

  const auto from_drawing = glm::mat4x3(
    S.r * Xr, S.g * Xg, S.b * Xb, 0.f,
    S.r * Yr, S.g * Yg, S.b * Yb, 0.f,
    S.r * Zr, S.g * Zg, S.b * Zb, 0.f);

  const auto to_monitor = glm::mat4x3(
  3.2404542f, -1.5371385f, -0.4985314f, 0,
  -0.9692660f,  1.8760108f,  0.0415560f, 0,
  0.0556434f, -0.2040259f,  1.0572252f, 0);

  pngine::conversion cnv_host_buffer;
  cnv_host_buffer.image_colorspace = (from_drawing);
  cnv_host_buffer.monitor_colorspace = (to_monitor);

  const uint32_t offset = sizeof(pngine::MVP) * 2u * m_flight_count;
  void* conversion_mapping = reinterpret_cast<char*>(m_uniform_mapping) + offset;
  ::memcpy(conversion_mapping, &cnv_host_buffer, sizeof(pngine::MVP));
}

void pngine_core::Transfer_Image_HTOD(vk::Image dst, vk::Buffer src, vk::Extent2D size) {
  auto& device = m_instance.get_device();
  /* allocate transfer commands for copy image */
  vk::CommandBufferAllocateInfo alloc_desc{};
  alloc_desc.sType = vk::StructureType::eCommandBufferAllocateInfo;
  alloc_desc.commandPool = m_transfer_pool.get();
  alloc_desc.level = vk::CommandBufferLevel::ePrimary;
  alloc_desc.commandBufferCount = 1u;

  auto transfer_buffers = device.get().allocateCommandBuffers(alloc_desc);
  auto& transfer_buffer = transfer_buffers[0u];

  /* command buffer option */
  vk::CommandBufferBeginInfo begin_info{};
  begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  /* preparing to copy image */
  vk::ImageMemoryBarrier barrier_template{};
  barrier_template.sType = vk::StructureType::eImageMemoryBarrier;
  barrier_template.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  barrier_template.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  barrier_template.image = dst;
  barrier_template.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  /* our barriers */
  vk::ImageMemoryBarrier undef_to_transferDst = barrier_template, transferDst_to_shaderRD = barrier_template;

  undef_to_transferDst.oldLayout = vk::ImageLayout::eUndefined;
  undef_to_transferDst.newLayout = vk::ImageLayout::eTransferDstOptimal;
  undef_to_transferDst.srcAccessMask = vk::AccessFlags{};
  undef_to_transferDst.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

  transferDst_to_shaderRD.oldLayout = vk::ImageLayout::eTransferDstOptimal;
  transferDst_to_shaderRD.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
  transferDst_to_shaderRD.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  transferDst_to_shaderRD.dstAccessMask = vk::AccessFlagBits::eShaderRead;

  transfer_buffer.begin(begin_info);
  using enum vk::PipelineStageFlagBits;
  transfer_buffer.pipelineBarrier(
      eTopOfPipe, eTransfer, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &undef_to_transferDst);

  vk::BufferImageCopy copy_img_region{};
  copy_img_region.bufferOffset = 0u;
  copy_img_region.bufferRowLength = 0u;
  copy_img_region.bufferImageHeight = 0u;
  copy_img_region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u);
  copy_img_region.imageOffset = vk::Offset3D(0, 0, 0);
  copy_img_region.imageExtent = vk::Extent3D(size, 1u);

  transfer_buffer.copyBufferToImage(src, dst, vk::ImageLayout::eTransferDstOptimal, 1u, &copy_img_region);
  transfer_buffer.pipelineBarrier(
      eTransfer, eFragmentShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &transferDst_to_shaderRD);
  transfer_buffer.end();

  vk::SubmitInfo submit_info{};
  submit_info.pCommandBuffers = &transfer_buffer, submit_info.commandBufferCount = 1u;
  auto transfer_queue = device.get_transfer_queue();
  vk::Result r = transfer_queue.submit(1u, &submit_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось записать изорбражение в очередь передачи!");
  transfer_queue.waitIdle();
  device.get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);
}
void pngine_core::Update_Descriptor_Set_0(
    const std::vector<vk::UniqueDescriptorSet>& dst_sets,
    uint32_t mvp_offset,
    vk::ImageView image_view) {
  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    vk::DescriptorBufferInfo d_mvp_info(m_uniform_mvps[i].get(), mvp_offset, sizeof(pngine::MVP));
    vk::DescriptorImageInfo d_texture_info({}, image_view, vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::WriteDescriptorSet w_mvp{}, w_texture{};
    w_mvp.sType = vk::StructureType::eWriteDescriptorSet;
    w_mvp.descriptorCount = 1u;
    w_mvp.pBufferInfo = &d_mvp_info;
    w_mvp.descriptorType = vk::DescriptorType::eUniformBuffer;
    w_mvp.dstSet = dst_sets[i].get();
    w_mvp.dstBinding = 0u;
    w_mvp.dstArrayElement = 0u;

    w_texture.sType = vk::StructureType::eWriteDescriptorSet;
    w_texture.descriptorCount = 1u;
    w_texture.pImageInfo = &d_texture_info;
    w_texture.descriptorType = vk::DescriptorType::eSampledImage;
    w_texture.dstSet = dst_sets[i].get();
    w_texture.dstBinding = 1u;
    w_texture.dstArrayElement = 0u;

    const auto sets = std::array{w_mvp, w_texture};
    m_instance.get_device().get().updateDescriptorSets(sets.size(), sets.data(), 0u, nullptr);
  }
}
void pngine_core::Update_Descriptor_Set_1() {
  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    vk::DescriptorBufferInfo d_cnv_info(m_uniform_conversion.get(), 0u, sizeof(pngine::conversion));
    vk::DescriptorImageInfo d_image_sampler_info(m_image_sampler.get(), {}, {});

    vk::WriteDescriptorSet w_cnv{}, w_image_sampler{};
    w_cnv.sType = vk::StructureType::eWriteDescriptorSet;
    w_cnv.descriptorCount = 1u;
    w_cnv.pBufferInfo = &d_cnv_info;
    w_cnv.descriptorType = vk::DescriptorType::eUniformBuffer;
    w_cnv.dstSet = m_descr_sets_1[i].get();
    w_cnv.dstBinding = 0u;
    w_cnv.dstArrayElement = 0u;

    w_image_sampler.sType = vk::StructureType::eWriteDescriptorSet;
    w_image_sampler.descriptorCount = 1u;
    w_image_sampler.pImageInfo = &d_image_sampler_info;
    w_image_sampler.descriptorType = vk::DescriptorType::eSampler;
    w_image_sampler.dstSet = m_descr_sets_1[i].get();
    w_image_sampler.dstBinding = 1u;
    w_image_sampler.dstArrayElement = 0u;

    const auto sets = std::array{w_cnv, w_image_sampler};
    m_instance.get_device().get().updateDescriptorSets(sets.size(), sets.data(), 0u, nullptr);
  }
}

void pngine_core::Update_Image(uint32_t frame_index) {
  const auto& device = m_instance.get_device();
  const auto extent = device.get_swapchain().get_extent();

  const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
  const auto &w = m_png_info->width, &h = m_png_info->height;
  const auto big_side = std::max(w, h), little_side = std::min(w, h);
  const auto canvas_sides_ratio = static_cast<float>(little_side) / static_cast<float>(big_side);
  pngine::MVP mvp_host_buffer;
  if (w < h)
    mvp_host_buffer.model = glm::scale(glm::mat4(1.f), glm::vec3(canvas_sides_ratio, 1.f, 1.f));
  else
    mvp_host_buffer.model = glm::scale(glm::mat4(1.f), glm::vec3(1.f, canvas_sides_ratio, 1.f));

  if (extent.width < extent.height)
    mvp_host_buffer.model = glm::scale(mvp_host_buffer.model, glm::vec3(aspect, aspect, 1.f));

  mvp_host_buffer.view = glm::lookAt(glm::vec3{0.f, 1e-7f, 1.f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, 1.f});

  mvp_host_buffer.proj = glm::perspective(glm::radians(70.f), aspect, 0.1f, 10.f);
  mvp_host_buffer.proj[1][1] *= -1.f;
  const uint32_t offset = frame_index * sizeof(pngine::MVP) * 2u;
  void* drawing_mapping = reinterpret_cast<char*>(m_uniform_mapping) + offset;
  ::memcpy(drawing_mapping, &mvp_host_buffer, sizeof(pngine::MVP));
}

void pngine_core::Update_Menu(uint32_t frame_index) {
  const auto& device = m_instance.get_device();
  const auto extent = device.get_swapchain().get_extent();

  const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
  const auto w = 64u, h = 256u;
  const auto big_side = std::max(w, h), little_side = std::min(w, h);
  const auto big_extent = std::max(extent.width, extent.height);
  const auto canvas_sides_ratio = static_cast<float>(little_side) / static_cast<float>(big_side);
  pngine::MVP mvp_host_buffer;

  // масштабирование под разные формы изображений
  if (w < h)
    mvp_host_buffer.model = glm::scale(glm::mat4(1.f), glm::vec3(canvas_sides_ratio, 1.f, 1.f));
  else
    mvp_host_buffer.model = glm::scale(glm::mat4(1.f), glm::vec3(1.f, canvas_sides_ratio, 1.f));

  // умещение изображения под узкое окно
  if (extent.width >= extent.height)
    mvp_host_buffer.model = glm::scale(mvp_host_buffer.model, glm::vec3(aspect, aspect, 1.f));

  // предотвращение масштабирования элемента интерфейса при ресайзе окна
  const float floating_scale = static_cast<float>(big_side) / static_cast<float>(big_extent);
  mvp_host_buffer.model = glm::scale(mvp_host_buffer.model, glm::vec3(floating_scale, floating_scale, 1.f));

  // перемещаем элемент от центра окна к верхней левой границе
  const auto middle = glm::vec2(static_cast<float>(extent.width) / 2.f, static_cast<float>(extent.height) / 2.f);
  // также перемещаем центр элемента, чтобы он полностью отрисовался
  const float offset_edge_x = static_cast<float>(w) / 2.f, offset_edge_y = static_cast<float>(h) / 2.f;
  const float offset_x = (middle.x - offset_edge_x) / w, offset_y = (middle.y - offset_edge_y) / h;
  const auto translate = glm::translate(glm::mat4(1.f), glm::vec3(offset_x, -offset_y, 0.f));
  mvp_host_buffer.model *= translate;

  // матрицы вида и проекции
  constexpr const float fov = glm::radians(45.f); // угол обзора
  /*constexpr*/ const float z_position = 1.f / (std::tanf(fov / 2.f) * 2.f); // вычисление отдаления интерфейса
  mvp_host_buffer.view =
      glm::lookAt(glm::vec3{0.f, 1e-7f, z_position}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, 1.f});

  mvp_host_buffer.proj = glm::perspective(fov, aspect, 0.01f, 10.f);
  mvp_host_buffer.proj[1][1] *= -1.f;
  const uint32_t offset = frame_index * sizeof(pngine::MVP) * 2u;
  void* menu_mapping = reinterpret_cast<char*>(m_uniform_mapping) + offset + sizeof(pngine::MVP);
  ::memcpy(menu_mapping, &mvp_host_buffer, sizeof(pngine::MVP));
}

void pngine_core::render_frame() {
  auto& dev = m_instance.get_device();
  const auto& in_flight_f = m_in_flight_f[m_current_frame];
  const auto& image_available_s = m_image_available_s[m_current_frame];
  const auto& render_finished_s = m_render_finished_s[m_current_frame];
  const auto drawing_descr_sets =
      std::array{m_drawing_descr_sets_0[m_current_frame].get(), m_descr_sets_1[m_current_frame].get()};
  const auto menu_descr_sets =
      std::array{m_menu_descr_sets_0[m_current_frame].get(), m_descr_sets_1[m_current_frame].get()};
  const auto& cur_command_buffer = m_command_buffers[m_current_frame];
  constexpr const auto without_delays = std::numeric_limits<uint64_t>::max();
  vk::Result r = dev.get().waitForFences(1u, &in_flight_f, vk::True, without_delays);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не получилось синхронизировать устройство при помощи барьера!");
  const auto& swapchain = dev.get_swapchain();
  const uint32_t current_image =
      dev.get().acquireNextImageKHR(swapchain.get(), without_delays, image_available_s).value;
  const auto extent = dev.get_swapchain().get_extent();
  Update_Image(m_current_frame);
  Update_Menu(m_current_frame);
  r = dev.get().resetFences(1u, &in_flight_f);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось сбросить барьер!");

  vk::RenderPassBeginInfo render_pass_config{};
  render_pass_config.sType = vk::StructureType::eRenderPassBeginInfo;
  render_pass_config.renderPass = m_render_pass.get();
  render_pass_config.framebuffer = m_framebuffers.at(current_image).get();
  render_pass_config.renderArea.setOffset({0, 0});
  render_pass_config.renderArea.extent = extent;

  vk::ClearValue clear_color = vk::ClearColorValue{200.f / 256, 200.f / 256, 200.f / 256, 122.f / 256};
  const auto clear_values = std::array{clear_color, clear_color, clear_color};
  render_pass_config.pClearValues = clear_values.data();
  render_pass_config.clearValueCount = clear_values.size();

  vk::CommandBufferBeginInfo begin_desc{};
  begin_desc.sType = vk::StructureType::eCommandBufferBeginInfo;
  cur_command_buffer.reset();
  cur_command_buffer.begin(begin_desc);
  cur_command_buffer.beginRenderPass(render_pass_config, vk::SubpassContents::eInline);
  cur_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, dev.get_graphics_pipeline("basic_pipeline").get());
  vk::Viewport dynamic_viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f);
  vk::Rect2D dynamic_scissor({}, extent);

  cur_command_buffer.setViewport(0u, 1u, &dynamic_viewport);
  cur_command_buffer.setScissor(0u, 1u, &dynamic_scissor);
  constexpr const auto vertex_buffer_count = 1u;
  const vk::Buffer buffers[1] = {m_png_surface_mesh.get()};
  const vk::DeviceSize offsets[1] = {0u};
  cur_command_buffer.bindVertexBuffers(0u, vertex_buffer_count, buffers, offsets);
  cur_command_buffer.bindIndexBuffer(m_png_surface_ids.get(), 0u, vk::IndexType::eUint16);
  // draw drawing
  cur_command_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      m_pipeline_layout.get(),
      0u,
      drawing_descr_sets.size(),
      drawing_descr_sets.data(),
      0u,
      nullptr);
  uint32_t rectangle_indices = rectangle_ids.size(), inst_count = 1u, first_vert = 0u, first_inst = 0u;
  cur_command_buffer.drawIndexed(rectangle_indices, inst_count, first_vert, 0, first_inst);
  // draw menu
  cur_command_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      m_pipeline_layout.get(),
      0u,
      menu_descr_sets.size(),
      menu_descr_sets.data(),
      0u,
      nullptr);
  rectangle_indices = rectangle_ids.size(), inst_count = 1u, first_vert = 0u, first_inst = 0u;
  cur_command_buffer.drawIndexed(rectangle_indices, inst_count, first_vert, 0, first_inst);
  cur_command_buffer.endRenderPass();
  cur_command_buffer.end();

  vk::SubmitInfo submit_info{};
  submit_info.sType = vk::StructureType::eSubmitInfo;

  vk::PipelineStageFlags wait_masks[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
  submit_info.pWaitSemaphores = &image_available_s;
  submit_info.waitSemaphoreCount = 1u;
  submit_info.pWaitDstStageMask = wait_masks;

  submit_info.pSignalSemaphores = &render_finished_s;
  submit_info.signalSemaphoreCount = 1u;

  submit_info.pCommandBuffers = &cur_command_buffer;
  submit_info.commandBufferCount = 1u;

  r = dev.get_graphics_queue().submit(1u, &submit_info, in_flight_f);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось записать данные в графическую очередь!");

  vk::PresentInfoKHR present_info{};
  present_info.sType = vk::StructureType::ePresentInfoKHR;

  present_info.pWaitSemaphores = &render_finished_s;
  present_info.waitSemaphoreCount = 1u;

  vk::SwapchainKHR swapchain_ref = swapchain.get();
  present_info.pSwapchains = &swapchain_ref;
  present_info.swapchainCount = 1u;

  present_info.pImageIndices = &current_image;

  r = dev.get_present_queue().presentKHR(present_info);
  if (r == vk::Result::eErrorOutOfDateKHR || r == vk::Result::eSuboptimalKHR) {
#ifndef NDEBUG
    std::clog << "Pngine:[NOTIFICATION]: запись в present-очередь. Возвращает: " << vk::to_string(r) << '\n';
#endif
  } else if (r != vk::Result::eSuccess) {
    throw std::runtime_error("Pngine: не удалось произвести отображение на экране!");
  }
  m_current_frame = (m_current_frame + 1) % m_flight_count;
}

const char* pngine_core::get_engine_name() const noexcept {
  return pngine::bring_engine_name(); // статично, на этапе компиляции
}
pngine::human_version pngine_core::get_engine_version() const noexcept {
  return pngine::bring_human_version(bring_engine_version()); // статично, на этапе компиляции
}
std::string_view pngine_core::get_app_name() const noexcept {
  return m_app_name;
}
pngine::human_version pngine_core::get_vk_api_version() const noexcept {
  return pngine::bring_human_version(m_vk_api_version);
}
vk::Instance pngine_core::get_instance() const noexcept {
  return m_instance.get();
}
const pngine::image_manipulator& pngine_core::get_toolbox() const noexcept {
  return m_toolbox;
}
pngine::image_manipulator& pngine_core::get_toolbox() noexcept {
  return m_toolbox;
}
} // namespace pngine
} // namespace csc
