module;
#include <cstdint>
#include <unistd.h>
#include <bits/move.h>
#include <limits>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
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

import csc.pngine.instance;
import csc.pngine.instance.device;
import csc.pngine.window_handler;
import csc.pngine.glfw_handler;
import csc.pngine.vertex;

import csc.pngine.commons.utility.graphics_pipeline;

import csc.pngine.instance.device.command_pool;
import vulkan_hpp;

namespace csc {
namespace pngine {

const std::vector<pngine::vertex> rectangle = {
    pngine::vertex{{-0.5f, -0.5f}, {1.f, 1.f, 1.f}, {1.f, 0.f}},
    pngine::vertex{{0.5f, -0.5f}, {0.f, 0.f, 0.f}, {0.f, 0.f}},
    pngine::vertex{{0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, 1.f}},
    pngine::vertex{{-0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f, 1.f}}};
const std::vector<uint16_t> rectangle_ids = {0, 1, 2, 2, 3, 0};

// utility
vk::Format bring_texture_format_from_png(png::e_color_type type, uint8_t bit_depth) noexcept {
  using enum png::e_color_type;
  switch (type) {
    case rgba:
      return (bit_depth == 8) ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR16G16B16A16Unorm; // Srgb только rgba8
    case rgb:
      return (bit_depth == 8) ? vk::Format::eR8G8B8Unorm : vk::Format::eR16G16B16Unorm; // только линейное
    case bw:
      return (bit_depth == 8) ? vk::Format::eR8Unorm : vk::Format::eR16Unorm; // только линейное
    case bwa:
      return (bit_depth == 8) ? vk::Format::eR8G8Unorm : vk::Format::eR16G16Unorm; // только линейное
    case indexed:
      return (bit_depth == 8) ? vk::Format::eR8Unorm : vk::Format::eR16Unorm; // только линейное
    default:
      return vk::Format::eUndefined;
  }
}

// колбэки
static void recreate_swapchain(pngine::glfw_window* window, int, int) {
  auto& window_obj = *reinterpret_cast<pngine::window_handler*>(&window);
  auto& instance = *reinterpret_cast<pngine::instance*>(window_obj.get_user_pointer());
  auto& dev = instance.get_device();
  dev.get().waitIdle();
  dev.create_swapchainKHR(
      window_obj.get_framebuffer_size(), instance.get_swapchainKHR_dispatch()); // operator= очистит старый класс
  dev.create_image_views(); // operator= очистит старый класс
  dev.create_framebuffers("basic_pass"); // operator= очистит старый класс
}

export class pngine_core {
 private:
  std::string m_app_name{"Application"};
  pngine::version m_app_version = pngine::bring_version(1u, 0u, 0u);
  pngine::version m_vk_api_version = pngine::bring_version(1u, 3u, 256u);
  std::string m_gpu_name;
  const std::vector<uint8_t>* m_png_pixels = nullptr;

  const png::IHDR* m_png_info = nullptr;

  pngine::glfw_handler m_glfw_state{};
  pngine::window_handler m_window_handler;

  pngine::instance m_instance;
  uint32_t m_current_frame = 0u;
  static constexpr uint32_t m_flight_count = 2u;

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

  vk::UniqueDescriptorSetLayout m_descr_layout;
  vk::UniquePipelineLayout m_pipeline_layout;

  std::array<vk::UniqueBuffer, m_flight_count> m_uniform_mvps;
  std::array<vk::UniqueDeviceMemory, m_flight_count> m_uniform_mvp_memories;
  std::array<void*, m_flight_count> m_uniform_mvp_mappings;

  vk::UniqueDescriptorPool m_descr_pool;
  std::vector<vk::UniqueDescriptorSet> m_descr_sets;

  vk::UniqueBuffer m_stage_image_buffer{};
  vk::UniqueDeviceMemory m_stage_image_memory{};
  vk::UniqueImage m_image_buffer{};
  vk::UniqueImageView m_image_view{};
  vk::UniqueSampler m_image_sampler{};
  vk::UniqueDeviceMemory m_image_memory{};

  pngine::command_pool m_graphics_pool;
  pngine::command_pool m_transfer_pool;
  std::vector<vk::CommandBuffer> m_command_buffers;

  void Render_Frame();
  void Load_Mesh();
  void Load_Textures();
  void Update(uint32_t frame_index);

 public:
  pngine_core() = delete;
  pngine_core(std::string app_name, pngine::version app_version, std::string gpu_name);
  void set_drawing(const std::vector<uint8_t>& blob, const png::IHDR& img_info);
  void run();
  ~pngine_core() noexcept;

  const char* get_engine_name() const noexcept;
  pngine::human_version get_engine_version() const noexcept;
  pngine::human_version get_vk_api_version() const noexcept;
  std::string_view get_app_name() const noexcept;
};

pngine_core::pngine_core(std::string nm, pngine::version ver, std::string g_nm)
    : m_app_name(std::move(nm)), m_app_version(ver), m_gpu_name(std::move(g_nm)) {
  vk::ApplicationInfo app_info(
      m_app_name.c_str(), m_app_version, pngine::bring_engine_name(), pngine::bring_engine_version(), m_vk_api_version);
  // высокоуровневый алгоритм...
  m_window_handler = pngine::window_handler(1280u, 720u, m_app_name);
  m_instance = pngine::instance(app_info);
  m_window_handler.set_user_pointer(&m_instance);
  m_window_handler.set_framebuffer_size_callback(recreate_swapchain);
  m_window_handler.set_size_limits(vk::Extent2D{320u, 240u});
#ifndef NDEBUG
  m_instance.create_debug_reportEXT();
#endif
  m_instance.create_surfaceKHR(m_window_handler);
  m_instance.bring_physical_devices();
  auto& device = m_instance.create_device(m_gpu_name);
  device.create_swapchainKHR(m_window_handler.get_framebuffer_size(), m_instance.get_swapchainKHR_dispatch());
  device.create_image_views();
  device.create_shader_module("vs_triangle", pngine::shaders::shaders_triangle::vert_path);
  device.create_shader_module("fs_triangle", pngine::shaders::shaders_triangle::frag_path);

  /* descriptor layouts */
  vk::DescriptorSetLayoutBinding u0_bind(
      0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr);
  vk::DescriptorSetLayoutBinding s1_bind(
      1u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr);
  const auto binds = std::array{u0_bind, s1_bind}; // deduction guides!

  vk::DescriptorSetLayoutCreateInfo descriptor_layout_info{};
  descriptor_layout_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
  descriptor_layout_info.bindingCount = binds.size();
  descriptor_layout_info.pBindings = binds.data();

  m_descr_layout = device.get().createDescriptorSetLayoutUnique(descriptor_layout_info, nullptr);

  vk::PipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  pipeline_layout_info.setLayoutCount = 1u;
  pipeline_layout_info.pSetLayouts = &m_descr_layout.get();
  m_pipeline_layout = device.get().createPipelineLayoutUnique(pipeline_layout_info, nullptr);

  /* render pass */
  device.create_render_pass("basic_pass");

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
  device.create_pipeline("basic_pipeline", m_pipeline_layout, "basic_pass", triangle_cfg);
  device.create_framebuffers("basic_pass");
  /* vertex buffer */
  const uint32_t vtx_buf_size = sizeof(pngine::vertex) * rectangle.size();
  std::tie(m_stage_png_surface_mesh, m_stage_png_surface_mesh_memory) = device.create_buffer(
      vtx_buf_size,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  void* data =
      device.get().mapMemory(m_stage_png_surface_mesh_memory.get(), 0u, rectangle.size() * sizeof(pngine::vertex));
  ::memcpy(data, rectangle.data(), vtx_buf_size);
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

  data = device.get().mapMemory(m_stage_png_surface_ids_memory.get(), 0u, rectangle.size() * sizeof(pngine::vertex));
  ::memcpy(data, rectangle_ids.data(), idx_buf_size);
  device.get().unmapMemory(m_stage_png_surface_ids_memory.get());

  std::tie(m_png_surface_ids, m_png_surface_ids_memory) = device.create_buffer(
      idx_buf_size,
      vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
      vk::MemoryPropertyFlagBits::eDeviceLocal);
  /* uniform buffers */
  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    std::tie(m_uniform_mvps[i], m_uniform_mvp_memories[i]) = device.create_buffer(
        sizeof(pngine::MVP),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    m_uniform_mvp_mappings[i] = device.get().mapMemory(m_uniform_mvp_memories[i].get(), 0u, sizeof(pngine::MVP));
  }

  /* creating pools */
  m_graphics_pool = device.create_graphics_command_pool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
  m_transfer_pool = device.create_transfer_command_pool(vk::CommandPoolCreateFlagBits::eTransient);

  m_command_buffers = m_graphics_pool.allocate_command_buffers(vk::CommandBufferLevel::ePrimary, m_flight_count);

  /* descriptor pool */
  vk::DescriptorPoolSize mvp_pool_size(vk::DescriptorType::eUniformBuffer, m_flight_count);
  vk::DescriptorPoolSize image_sampler_pool_size(vk::DescriptorType::eCombinedImageSampler, m_flight_count);
  const auto pool_sizes = std::array{mvp_pool_size, image_sampler_pool_size};

  vk::DescriptorPoolCreateInfo des_pool_info{};
  des_pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
  des_pool_info.pPoolSizes = pool_sizes.data();
  des_pool_info.poolSizeCount = pool_sizes.size();
  des_pool_info.maxSets = m_flight_count;
  des_pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

  m_descr_pool = device.get().createDescriptorPoolUnique(des_pool_info, nullptr);

  std::array<vk::DescriptorSetLayout, m_flight_count> layouts;
  std::ranges::fill(layouts, m_descr_layout.get());
  vk::DescriptorSetAllocateInfo alloc_sets_info{};
  alloc_sets_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
  alloc_sets_info.descriptorPool = m_descr_pool.get();
  alloc_sets_info.descriptorSetCount = layouts.size();
  alloc_sets_info.pSetLayouts = layouts.data();

  m_descr_sets = device.get().allocateDescriptorSetsUnique(alloc_sets_info);

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

void pngine_core::set_drawing(const std::vector<uint8_t>& blob, const png::IHDR& img_info) {
  auto& device = m_instance.get_device();
  m_png_info = &img_info, m_png_pixels = &blob;

  /* image data */
  std::tie(m_stage_image_buffer, m_stage_image_memory) = device.create_buffer(
      blob.size(),
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  void* staged_image_mapping = device.get().mapMemory(m_stage_image_memory.get(), 0u, blob.size());
  ::memcpy(staged_image_mapping, blob.data(), blob.size());
  device.get().unmapMemory(m_stage_image_memory.get());

  std::tie(m_image_buffer, m_image_memory) = device.create_image(
      vk::Extent2D(m_png_info->width, m_png_info->height),
      pngine::bring_texture_format_from_png(m_png_info->color_type, m_png_info->bit_depth),
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      vk::MemoryPropertyFlagBits::eDeviceLocal);
  /* image view */
  vk::ImageViewCreateInfo image_view_info{};
  image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  image_view_info.image = m_image_buffer.get();
  image_view_info.format = pngine::bring_texture_format_from_png(m_png_info->color_type, m_png_info->bit_depth);
  image_view_info.viewType = vk::ImageViewType::e2D;
  image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  m_image_view = device.get().createImageViewUnique(image_view_info, nullptr);
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

  m_image_sampler = device.get().createSamplerUnique(sampler_info, nullptr);
  /* setup descriptor sets */
  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    vk::DescriptorBufferInfo d_mvp_info(m_uniform_mvps[i].get(), 0u, sizeof(pngine::MVP));
    vk::DescriptorImageInfo d_image_sampler_info(
        m_image_sampler.get(), m_image_view.get(), vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::WriteDescriptorSet write_mvp{}, write_image_sampler{};
    write_mvp.sType = vk::StructureType::eWriteDescriptorSet;
    write_mvp.descriptorCount = 1u;
    write_mvp.pBufferInfo = &d_mvp_info;
    write_mvp.descriptorType = vk::DescriptorType::eUniformBuffer;
    write_mvp.dstSet = m_descr_sets[i].get();
    write_mvp.dstBinding = 0u;
    write_mvp.dstArrayElement = 0u;

    write_image_sampler.sType = vk::StructureType::eWriteDescriptorSet;
    write_image_sampler.descriptorCount = 1u;
    write_image_sampler.pImageInfo = &d_image_sampler_info;
    write_image_sampler.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write_image_sampler.dstSet = m_descr_sets[i].get();
    write_image_sampler.dstBinding = 1u;
    write_image_sampler.dstArrayElement = 0u;

    const auto writings = std::array{write_mvp, write_image_sampler};
    device.get().updateDescriptorSets(writings.size(), writings.data(), 0u, nullptr);
  }
}

void pngine_core::run() {
  Load_Mesh(); // присылаем данные перед рендерингом
  Load_Textures(); // присылаем текстуру картинки перед рендерингом
  double time = 0.0;
  uint64_t frames_count = 0u;
  while (!m_window_handler.should_close()) {
    m_glfw_state.poll_events();
    const auto start = std::chrono::high_resolution_clock::now();
    Render_Frame();
    const auto end = std::chrono::high_resolution_clock::now();
    time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
    frames_count += 1u;
    if (time >= 1.0) {
      std::cout << "Render frames per second: " << frames_count << '\n';
      frames_count = 0u, time = 0.0;
    }
  }
}
void pngine_core::Load_Mesh() {
  const auto& device = m_instance.get_device();
  auto transfer_buffer = std::move(m_transfer_pool.allocate_command_buffers(vk::CommandBufferLevel::ePrimary, 1u)[0]);

  vk::CommandBufferBeginInfo begin_info{};
  begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  transfer_buffer.begin(begin_info);
  vk::BufferCopy copy_vtx_region(0u, 0u, sizeof(pngine::vertex) * rectangle.size());
  transfer_buffer.copyBuffer(m_stage_png_surface_mesh.get(), m_png_surface_mesh.get(), 1u, &copy_vtx_region);
  vk::BufferCopy copy_idx_region(0u, 0u, sizeof(uint16_t) * rectangle_ids.size());
  transfer_buffer.copyBuffer(m_stage_png_surface_ids.get(), m_png_surface_ids.get(), 1u, &copy_idx_region);
  transfer_buffer.end();

  vk::SubmitInfo submit_info{};
  submit_info.sType = vk::StructureType::eSubmitInfo;
  submit_info.pCommandBuffers = &transfer_buffer;
  submit_info.commandBufferCount = 1u;
  auto transfer_queue = device.get_transfer_queue();
  vk::Result r = transfer_queue.submit(1u, &submit_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось записать вершинный буфер в очередь передачи!");
  transfer_queue.waitIdle();
  device.get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);
}
void pngine_core::Load_Textures() {
  const auto& device = m_instance.get_device();
  auto transfer_buffer = std::move(m_transfer_pool.allocate_command_buffers(vk::CommandBufferLevel::ePrimary, 1u)[0]);

  vk::CommandBufferBeginInfo begin_info{};
  begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  /* preparing to copy image */
  vk::ImageMemoryBarrier barrier_template{};
  barrier_template.sType = vk::StructureType::eImageMemoryBarrier;
  barrier_template.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  barrier_template.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  barrier_template.image = m_image_buffer.get();
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
  // copy_img_region.imageOffset = vk::Extent3D(0u, 0u, 0u);
  copy_img_region.imageExtent = vk::Extent3D(m_png_info->width, m_png_info->height, 1u);

  transfer_buffer.copyBufferToImage(
      m_stage_image_buffer.get(), m_image_buffer.get(), vk::ImageLayout::eTransferDstOptimal, 1u, &copy_img_region);
  transfer_buffer.pipelineBarrier(
      eTransfer, eFragmentShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &transferDst_to_shaderRD);
  transfer_buffer.end();

  vk::SubmitInfo submit_info{};
  submit_info.pCommandBuffers = &transfer_buffer, submit_info.commandBufferCount = 1u;
  auto transfer_queue = device.get_transfer_queue();
  vk::Result r = transfer_queue.submit(1u, &submit_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось записать вершинный буфер в очередь передачи!");
  transfer_queue.waitIdle();
  device.get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);
}

void pngine_core::Update(uint32_t frame_index) {
  const auto& device = m_instance.get_device();
  const auto extent = device.get_swapchain().get_extent();
  const auto& current_mapping = m_uniform_mvp_mappings[frame_index];

  const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
  const auto &w = m_png_info->width, &h = m_png_info->height;
  const auto big_side = std::max(w, h), little_side = std::min(w, h);
  const auto canvas_sides_ratio = static_cast<float>(little_side) / static_cast<float>(big_side);
  pngine::MVP mvp_host_buffer;
  if (w < h)
    mvp_host_buffer.model = glm::scale(glm::mat4(1.f), glm::vec3(canvas_sides_ratio, 1.f, 1.f));
  else
    mvp_host_buffer.model = glm::scale(glm::mat4(1.f), glm::vec3(1.f, canvas_sides_ratio, 1.f));

  mvp_host_buffer.view = glm::lookAt(glm::vec3{0.f, 1e-7f, 1.f}, glm::vec3{0.0f, 0.f, 0.f}, glm::vec3{0.f, 0.f, 1.f});
  mvp_host_buffer.proj = glm::perspective(glm::radians(70.f), aspect, 0.1f, 10.f);
  mvp_host_buffer.proj[1][1] *= -1.f;
  ::memcpy(current_mapping, &mvp_host_buffer, sizeof(pngine::MVP));
}

void pngine_core::Render_Frame() {
  auto& dev = m_instance.get_device();
  const auto& in_flight_f = m_in_flight_f[m_current_frame];
  const auto& image_available_s = m_image_available_s[m_current_frame];
  const auto& render_finished_s = m_render_finished_s[m_current_frame];
  const auto& cur_descr_set = m_descr_sets[m_current_frame];
  const auto& cur_command_buffer = m_command_buffers[m_current_frame];
  constexpr const auto without_delays = std::numeric_limits<uint64_t>::max();
  vk::Result r = dev.get().waitForFences(1u, &in_flight_f, vk::True, without_delays);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не получилось синхронизировать устройство при помощи барьера!");
  const auto& swapchain = dev.get_swapchain();
  const uint32_t current_image =
      dev.get().acquireNextImageKHR(swapchain.get(), without_delays, image_available_s).value;
  const auto extent = dev.get_swapchain().get_extent();
  Update(m_current_frame);
  r = dev.get().resetFences(1u, &in_flight_f);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось сбросить барьер!");

  vk::RenderPassBeginInfo render_pass_config{};
  render_pass_config.sType = vk::StructureType::eRenderPassBeginInfo;
  render_pass_config.renderPass = dev.get_render_pass("basic_pass").get();
  render_pass_config.framebuffer = dev.get_framebuffers().at(current_image).get();
  render_pass_config.renderArea.setOffset({0, 0});
  render_pass_config.renderArea.extent = extent;

  vk::ClearValue clear_color = vk::ClearColorValue{200.f / 256, 200.f / 256, 200.f / 256, 12.f / 256};
  render_pass_config.pClearValues = &clear_color;
  render_pass_config.clearValueCount = 1u;

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
  cur_command_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, m_pipeline_layout.get(), 0u, 1u, &cur_descr_set.get(), 0u, nullptr);
  const uint32_t rectangle_indices = rectangle_ids.size(), inst_count = 1u, first_vert = 0u, first_inst = 0u;
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
  if (r == vk::Result::eErrorOutOfDateKHR || r == vk::Result::eSuboptimalKHR)
    std::clog << "Pngine:[NOTIFICATION]: запись в present-очередь. Возвращает: " << vk::to_string(r) << '\n';
  else if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось произвести отображение на экране!");
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

} // namespace pngine
} // namespace csc
