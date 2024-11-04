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
    pngine::vertex{{-0.5f, -0.5f}, {1.f, 1.f, 1.f}},
    pngine::vertex{{0.5f, -0.5f}, {0.f, 0.f, 0.f}},
    pngine::vertex{{0.5f, 0.5f}, {0.f, 0.f, 0.f}},
    pngine::vertex{{-0.5f, 0.5f}, {1.f, 1.f, 1.f}},
};
const std::vector<uint16_t> rectangle_ids = {0, 1, 2, 2, 3, 0};

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

  pngine::command_pool m_graphics_pool;
  pngine::command_pool m_transfer_pool;
  std::vector<vk::CommandBuffer> m_command_buffers;

  void Render_Frame();
  void Load_Mesh();
  void Update(uint32_t frame_index);

 public:
  pngine_core() = delete;
  pngine_core(std::string app_name, pngine::version app_version, std::string gpu_name);
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

  vk::DescriptorSetLayoutBinding descriptor_layout_binding{};
  descriptor_layout_binding.binding = 0u;
  descriptor_layout_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
  descriptor_layout_binding.descriptorCount = 1u;
  descriptor_layout_binding.pImmutableSamplers = nullptr;
  descriptor_layout_binding.stageFlags = vk::ShaderStageFlagBits::eVertex;

  vk::DescriptorSetLayoutCreateInfo descriptor_layout_info{};
  descriptor_layout_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
  descriptor_layout_info.bindingCount = 1u;
  descriptor_layout_info.pBindings = &descriptor_layout_binding;

  m_descr_layout = device.get().createDescriptorSetLayoutUnique(descriptor_layout_info, nullptr);

  vk::PipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  pipeline_layout_info.setLayoutCount = 1u;
  pipeline_layout_info.pSetLayouts = &m_descr_layout.get();
  m_pipeline_layout = device.get().createPipelineLayoutUnique(pipeline_layout_info, nullptr);

  device.create_render_pass("basic_pass");

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

  /* descriptor sets */
  vk::DescriptorPoolSize pool_size(vk::DescriptorType::eUniformBuffer, m_flight_count);
  vk::DescriptorPoolCreateInfo des_pool_info{};
  des_pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
  des_pool_info.pPoolSizes = &pool_size;
  des_pool_info.poolSizeCount = 1u;
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

  /* setup descriptor sets */
  for (uint32_t i = 0u; i < m_flight_count; ++i) {
    vk::DescriptorBufferInfo buf_info(m_uniform_mvps[i].get(), 0u, sizeof(pngine::MVP));
    vk::WriteDescriptorSet write{};
    write.sType = vk::StructureType::eWriteDescriptorSet;
    write.descriptorCount = 1u;
    write.pBufferInfo = &buf_info;
    write.descriptorType = vk::DescriptorType::eUniformBuffer;
    write.dstSet = m_descr_sets[i].get();
    write.dstBinding = 0u;
    write.dstArrayElement = 0u;
    device.get().updateDescriptorSets(1u, &write, 0u, nullptr);
  }

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
void pngine_core::run() {
  Load_Mesh(); // присылаем данные перед рендерингом
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
void pngine_core::Update(uint32_t frame_index) {
  const auto& device = m_instance.get_device();
  const auto extent = device.get_swapchain().get_extent();
  const auto& current_mapping = m_uniform_mvp_mappings[frame_index];

  const float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
  pngine::MVP mvp_host_buffer;
  mvp_host_buffer.model = glm::mat4{1.0};
  mvp_host_buffer.view = glm::lookAt(glm::vec3{1e-7f, 0.f, 1.f}, glm::vec3{0.f, 0.f, -2.f}, glm::vec3{0.f, 0.f, 1.f});
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
