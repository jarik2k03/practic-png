module;
#include <cstdint>
#include <unistd.h>
#include <bits/move.h>
#include <limits>
#include <iostream>
#include <shaders_triangle.h>
export module csc.pngine;

export import :attributes;

import stl.vector;
import stl.array;
import stl.string;
export import stl.string_view;
import stl.stdexcept;
import stl.variant;
#ifndef NDEBUG
import stl.iostream;
#endif
import csc.pngine.instance;
import csc.pngine.instance.device;
import csc.pngine.window_handler;

import csc.pngine.commons.utility.graphics_pipeline;
import vulkan_hpp;

namespace csc {
namespace pngine {

pngine::v_window_handler init_window_handler(uint32_t width, uint32_t height, std::string_view name) noexcept {
#ifdef __linux
  return pngine::v_window_handler(pngine::xcb_handler(width, height, name));
#elif _WIN32
  return pngine::v_window_handler(pngine::win32_handler(width, height, name));
#endif
}

} // namespace pngine
} // namespace csc

export namespace csc {
namespace pngine {

class pngine_core {
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

  pngine::v_window_handler m_window_handler;
  std::vector<vk::CommandBuffer> m_command_buffers;

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
  m_window_handler = pngine::init_window_handler(1050u, 450u, m_app_name);
  m_instance = pngine::instance(app_info);
#ifndef NDEBUG
  m_instance.create_debug_reportEXT();
#endif
  m_instance.create_surfaceKHR(m_window_handler);
  m_instance.bring_physical_devices();
  auto& device = m_instance.create_device(m_gpu_name);
  device.create_swapchainKHR(m_instance.get_swapchainKHR_dispatch());
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
  triangle_cfg.dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  device.create_pipeline("basic_pipeline", "basic_layout", "basic_pass", triangle_cfg);
  device.create_framebuffers("basic_pass");
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
}
void pngine_core::run() {
  auto& dev = m_instance.get_device();
  auto recreate_swapchain = [&]() -> void {
    dev.get().waitIdle();
    dev.create_swapchainKHR(m_instance.get_swapchainKHR_dispatch()); // operator= очистит старый класс
    dev.create_image_views(); // operator= очистит старый класс
    dev.create_framebuffers("basic_pass"); // operator= очистит старый класс
    std::cout << "Событие обработано.\n";
  };
  while (std::visit(pngine::f_wait_events(), m_window_handler)) {
    const auto& in_flight_f = m_in_flight_f[m_current_frame];
    const auto& image_available_s = m_image_available_s[m_current_frame];
    const auto& render_finished_s = m_render_finished_s[m_current_frame];
    const auto& cur_command_buffer = m_command_buffers[m_current_frame];
    constexpr const auto without_delays = std::numeric_limits<uint64_t>::max();
    std::visit(pngine::f_size_event(recreate_swapchain), m_window_handler); // событие изменения окна
    vk::Result r = dev.get().waitForFences(1u, &in_flight_f, vk::True, without_delays);
    if (r != vk::Result::eSuccess)
      throw std::runtime_error("Pngine: не получилось синхронизировать устройство при помощи барьера!");
    r = dev.get().resetFences(1u, &in_flight_f);
    if (r != vk::Result::eSuccess)
      throw std::runtime_error("Pngine: не удалось сбросить барьер!");
    const auto& swapchain = dev.get_swapchain();
    const uint32_t current_image =
        dev.get().acquireNextImageKHR(swapchain.get(), without_delays, image_available_s).value;

    vk::RenderPassBeginInfo render_pass_config{};
    render_pass_config.sType = vk::StructureType::eRenderPassBeginInfo;
    render_pass_config.renderPass = dev.get_render_pass("basic_pass").get();
    render_pass_config.framebuffer = dev.get_framebuffers().at(current_image).get();
    render_pass_config.renderArea.setOffset({0, 0});
    render_pass_config.renderArea.extent = dev.get_swapchain().get_extent();

    vk::ClearValue clear_color = vk::ClearColorValue{70.f / 256, 70.f / 256, 70.f / 256, 12.f / 256};
    render_pass_config.pClearValues = &clear_color;
    render_pass_config.clearValueCount = 1u;

    vk::CommandBufferBeginInfo begin_desc{};
    begin_desc.sType = vk::StructureType::eCommandBufferBeginInfo;
    cur_command_buffer.reset();
    cur_command_buffer.begin(begin_desc);
    cur_command_buffer.beginRenderPass(render_pass_config, vk::SubpassContents::eInline);
    cur_command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, dev.get_graphics_pipeline("basic_pipeline").get());
    const auto extent = dev.get_swapchain().get_extent();
    vk::Viewport dynamic_viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f);
    vk::Rect2D dynamic_scissor({}, extent);

    cur_command_buffer.setViewport(0u, 1u, &dynamic_viewport);
    cur_command_buffer.setScissor(0u, 1u, &dynamic_scissor);
    constexpr const uint32_t triangle_vertices = 3u, inst_count = 1u, first_vert = 0u, first_inst = 0u;
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
    if (r != vk::Result::eSuccess)
      throw std::runtime_error("Pngine: не удалось произвести отображение на экране!");
    m_current_frame = (m_current_frame + 1) % m_flight_count;
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

} // namespace pngine
} // namespace csc
