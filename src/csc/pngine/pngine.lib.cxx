module;
#include <cstdint>
#include <unistd.h>
#include <bits/move.h>
#include <limits>
#include <cstring>
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
import vulkan_hpp;

namespace csc {
namespace pngine {

const std::vector<pngine::vertex> triangle = {
    pngine::vertex{{-0.35f, -0.25f}, {1.f, 1.f, 1.f}},
    pngine::vertex{{0.35f, -0.25f}, {1.f, 1.f, 1.f}},
    pngine::vertex{{-0.35f, 0.25f}, {0.f, 0.f, 0.f}},

    pngine::vertex{{-0.35f, 0.5f}, {1.f, 0.f, 0.f}},
    pngine::vertex{{0.35f, 0.5f}, {0.f, 1.f, 0.f}},
    pngine::vertex{{0.35f, -0.5f}, {0.f, 0.f, 1.f}}};
uint32_t choose_memory_type(
    vk::PhysicalDeviceMemoryProperties supported,
    vk::MemoryRequirements specified,
    vk::MemoryPropertyFlags required) {
  for (uint32_t i = 0u; i < supported.memoryTypeCount; ++i) {
    if ((specified.memoryTypeBits & (1 << i)) && (supported.memoryTypes[i].propertyFlags & required) == required)
      return i;
  }
  throw std::runtime_error("Pngine: типы памяти не поддерживаются для текущей видеокарты!");
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
  pngine::instance m_instance;
  uint32_t m_current_frame = 0u;
  static constexpr uint32_t m_flight_count = 2u;

  std::array<vk::Semaphore, m_flight_count> m_image_available_s;
  std::array<vk::Semaphore, m_flight_count> m_render_finished_s;
  std::array<vk::Fence, m_flight_count> m_in_flight_f;
  vk::UniqueBuffer m_png_surface_mesh;
  vk::UniqueDeviceMemory m_png_surface_mesh_memory;
  pngine::glfw_handler m_glfw_state{};
  pngine::window_handler m_window_handler;
  std::vector<vk::CommandBuffer> m_command_buffers;

  void Render_Frame();

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
  m_window_handler = pngine::window_handler(1050u, 450u, m_app_name);
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
  device.create_pipeline_layout("basic_layout");
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
  device.create_pipeline("basic_pipeline", "basic_layout", "basic_pass", triangle_cfg);
  device.create_framebuffers("basic_pass");

  vk::BufferCreateInfo vertex_buffer_desc{};
  vertex_buffer_desc.sharingMode = vk::SharingMode::eExclusive;
  vertex_buffer_desc.usage = vk::BufferUsageFlagBits::eVertexBuffer;
  vertex_buffer_desc.size = triangle.size() * sizeof(pngine::vertex); // для обычного треугольника
  m_png_surface_mesh = device.get().createBufferUnique(vertex_buffer_desc, nullptr);
  const auto spec_props = device.get().getBufferMemoryRequirements(m_png_surface_mesh.get());
  const auto supported_props = device.get_physdev().getMemoryProperties();
  const auto required_props = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;

  vk::MemoryAllocateInfo alloc_info{};
  alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
  alloc_info.allocationSize = spec_props.size;
  alloc_info.memoryTypeIndex = pngine::choose_memory_type(supported_props, spec_props, required_props);
  m_png_surface_mesh_memory = device.get().allocateMemoryUnique(alloc_info);

  device.get().bindBufferMemory(m_png_surface_mesh.get(), m_png_surface_mesh_memory.get(), 0u);

  void* data = device.get().mapMemory(m_png_surface_mesh_memory.get(), 0u, triangle.size() * sizeof(pngine::vertex));
  ::memcpy(data, triangle.data(), triangle.size() * sizeof(pngine::vertex));
  device.get().unmapMemory(m_png_surface_mesh_memory.get());

  auto& command_pool = device.create_command_pool();
  m_command_buffers = command_pool.allocate_command_buffers(vk::CommandBufferLevel::ePrimary, m_flight_count);

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
  // m_png_surface_mesh.release();
}
void pngine_core::run() {
  while (!m_window_handler.should_close()) {
    m_glfw_state.poll_events();
    const auto start = std::chrono::high_resolution_clock::now();
    Render_Frame();
    const auto end = std::chrono::high_resolution_clock::now();
    const double time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
    // std::cout << "Render frame time: " << time << "; Framerate: " << 1 / time << '\n';
  }
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

void pngine_core::Render_Frame() {
  auto& dev = m_instance.get_device();
  const auto& in_flight_f = m_in_flight_f[m_current_frame];
  const auto& image_available_s = m_image_available_s[m_current_frame];
  const auto& render_finished_s = m_render_finished_s[m_current_frame];
  const auto& cur_command_buffer = m_command_buffers[m_current_frame];
  constexpr const auto without_delays = std::numeric_limits<uint64_t>::max();
  vk::Result r = dev.get().waitForFences(1u, &in_flight_f, vk::True, without_delays);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не получилось синхронизировать устройство при помощи барьера!");
  r = dev.get().resetFences(1u, &in_flight_f);
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Pngine: не удалось сбросить барьер!");
  const auto& swapchain = dev.get_swapchain();
  const uint32_t current_image =
      dev.get().acquireNextImageKHR(swapchain.get(), without_delays, image_available_s).value;
  const auto extent = dev.get_swapchain().get_extent();

  vk::RenderPassBeginInfo render_pass_config{};
  render_pass_config.sType = vk::StructureType::eRenderPassBeginInfo;
  render_pass_config.renderPass = dev.get_render_pass("basic_pass").get();
  render_pass_config.framebuffer = dev.get_framebuffers().at(current_image).get();
  render_pass_config.renderArea.setOffset({0, 0});
  render_pass_config.renderArea.extent = extent;

  vk::ClearValue clear_color = vk::ClearColorValue{70.f / 256, 70.f / 256, 70.f / 256, 12.f / 256};
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
  const uint32_t triangle_vertices = triangle.size(), inst_count = 1u, first_vert = 0u, first_inst = 0u;
  cur_command_buffer.draw(triangle_vertices, inst_count, first_vert, first_inst);
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

} // namespace pngine
} // namespace csc
