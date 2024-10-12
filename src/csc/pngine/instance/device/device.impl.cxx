module;
#include <utility>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <map>
export module csc.pngine.instance.device:impl;

export import vulkan_hpp;

import stl.stdexcept;
import stl.vector;
import stl.optional;
import stl.string;
import stl.string_view;

import csc.pngine.instance.device.queues;
import csc.pngine.instance.device.swapchainKHR;
import csc.pngine.instance.device.image_view;
import csc.pngine.instance.device.shader_module;
import csc.pngine.instance.device.graphics_pipeline;
import csc.pngine.instance.device.pipeline_layout;
import csc.pngine.instance.device.render_pass;
import csc.pngine.instance.device.framebuffer;

import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.commons.utility.queue_family_indices;
import csc.pngine.commons.utility.graphics_pipeline;

import :utility;
namespace csc {
namespace pngine {
class device_impl {
 private:
  vk::Device m_device{};
  pngine::queue_family_indices m_indices{};
  const vk::PhysicalDevice* m_keep_phdevice{};
  const vk::SurfaceKHR* m_keep_surface{};

  pngine::swapchainKHR m_swapchain{};
  std::vector<pngine::image_view> m_image_views{};
  std::map<std::string, pngine::shader_module> m_shader_modules{};
  std::vector<pngine::graphics_pipeline> m_pipelines{};
  std::map<std::string, pngine::pipeline_layout> m_pipeline_layouts{};
  std::map<std::string, pngine::render_pass> m_render_passes{};
  std::vector<pngine::framebuffer> m_framebuffers{};

  std::vector<const char*> m_enabled_extensions{};
  vk::Bool32 m_is_created = false;

 public:
  explicit device_impl() = default;
  ~device_impl() noexcept;
  device_impl(device_impl&& move) noexcept = default;
  device_impl& operator=(device_impl&& move) noexcept;
  explicit device_impl(const vk::PhysicalDevice& dev, const vk::SurfaceKHR& surface);
  void do_clear() noexcept;
  const pngine::shader_module& do_get_shader_module(std::string_view name) const;
  const pngine::swapchainKHR& do_get_swapchain() const;
  const pngine::render_pass& do_get_render_pass(std::string_view name) const;

  void do_create_swapchainKHR();
  void do_create_image_views();
  void do_create_shader_module(std::string_view name, std::string_view compiled_filepath);
  template <pngine::c_graphics_pipeline_config Config>
  pngine::graphics_pipeline&
  do_create_pipeline(std::string_view layout_name, std::string_view pass_name, Config&& config);
  void do_create_pipeline_layout(std::string_view layout_name);
  void do_create_render_pass(std::string_view pass_name);
  void do_create_framebuffers(std::string_view pass_name);
};

device_impl::~device_impl() noexcept {
  do_clear();
}
device_impl& device_impl::operator=(device_impl&& move) noexcept {
  if (this == &move)
    return *this;
  do_clear();
  m_device = move.m_device;
  m_indices = move.m_indices;
  m_keep_surface = move.m_keep_surface;
  m_keep_phdevice = move.m_keep_phdevice;

  m_swapchain = std::move(move.m_swapchain);
  m_image_views = std::move(move.m_image_views);
  m_shader_modules = std::move(move.m_shader_modules);
  m_pipelines = std::move(move.m_pipelines);
  m_pipeline_layouts = std::move(move.m_pipeline_layouts);
  m_render_passes = std::move(move.m_render_passes);
  m_framebuffers = std::move(move.m_framebuffers);
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
device_impl::device_impl(const vk::PhysicalDevice& dev, const vk::SurfaceKHR& surface)
    : m_keep_phdevice(&dev), m_keep_surface(&surface) {
  m_indices = pngine::bring_indices_from_phys_device(dev, surface);

  m_enabled_extensions.reserve(256ul);
  m_enabled_extensions.emplace_back("VK_KHR_swapchain");

  if (!m_indices.graphics.has_value())
    throw std::runtime_error("Device: Не найдено graphics-семейство очередей для устройства!");
  if (!m_indices.present.has_value())
    throw std::runtime_error("Device: Не найдено present-семейство очередей для поверхности окна!");

  std::vector<vk::DeviceQueueCreateInfo> q_descriptions;
  pngine::queues graphics_queues(m_indices.graphics.value(), {1.0});
  q_descriptions.emplace_back(graphics_queues.bring_info());
  if (m_indices.graphics.value() != m_indices.present.value()) {
    pngine::queues present_queues(m_indices.present.value(), {1.0}); // если индексы разные - то и очереди тоже
    q_descriptions.emplace_back(present_queues.bring_info());
  }

  vk::DeviceCreateInfo description{};
  description.sType = vk::StructureType::eDeviceCreateInfo;
  description.pNext = nullptr;
  description.ppEnabledExtensionNames = m_enabled_extensions.data();
  description.enabledExtensionCount = m_enabled_extensions.size();
  description.pQueueCreateInfos = q_descriptions.data();
  description.queueCreateInfoCount = q_descriptions.size();

  m_device = dev.createDevice(description);
  m_is_created = true; // если кинет исключение - не будет is created
}

void device_impl::do_create_swapchainKHR() {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать swapchainKHR, пока не создан device!");
  const auto details = pngine::bring_swapchain_details_from_phys_device(*m_keep_phdevice, *m_keep_surface);
  m_swapchain = pngine::swapchainKHR(m_device, *m_keep_surface, details, m_indices);
}

void device_impl::do_create_image_views() {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать image_view, пока не создан device!");
  m_image_views.reserve(m_swapchain.get_images().size());
  std::ranges::for_each(m_swapchain.get_images(), [&](const auto& img) {
    m_image_views.emplace_back(pngine::image_view(m_device, img, m_swapchain.get_image_format()));
  });
}
void device_impl::do_create_shader_module(std::string_view name, std::string_view compiled_filepath) {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать shader_module, пока не создан device!");
  m_shader_modules.insert(std::make_pair(name.data(), pngine::shader_module(m_device, compiled_filepath)));
}

template <pngine::c_graphics_pipeline_config Config>
pngine::graphics_pipeline&
device_impl::do_create_pipeline(std::string_view layout_name, std::string_view pass_name, Config&& config) {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать graphics_pipeline, пока не создан device!");

  const auto render_pass_pos = m_render_passes.find(pass_name.data());
  [[unlikely]] if (render_pass_pos == m_render_passes.cend())
    throw std::runtime_error("Device: не удалось найти указанный проход конвейера!");

  const auto pipeline_layout_pos = m_pipeline_layouts.find(layout_name.data());
  [[unlikely]] if (pipeline_layout_pos == m_pipeline_layouts.cend())
    throw std::runtime_error("Device: не удалось найти указанный макет конвейера!");

  m_pipelines.emplace_back(pngine::graphics_pipeline(
      m_device, render_pass_pos->second.get(), pipeline_layout_pos->second.get(), std::forward<Config>(config)));
  return m_pipelines.back();
}

void device_impl::do_create_pipeline_layout(std::string_view name) {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать pipeline_layout, пока не создан device!");
  m_pipeline_layouts.insert(std::make_pair(name.data(), pngine::pipeline_layout(m_device)));
}
void device_impl::do_create_render_pass(std::string_view name) {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать render_pass, пока не создан device!");
  m_render_passes.insert(std::make_pair(name.data(), pngine::render_pass(m_device, m_swapchain.get_image_format())));
}

void device_impl::do_create_framebuffers(std::string_view pass_name) {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно создать framebuffers, пока не создан device!");
  const auto render_pass_pos = m_render_passes.find(pass_name.data());
  [[unlikely]] if (render_pass_pos == m_render_passes.cend())
    throw std::runtime_error("Device: не удалось найти указанный проход рендера!");

  m_framebuffers.reserve(m_image_views.size());
  std::ranges::for_each(m_image_views, [&](const auto& img) {
    m_framebuffers.emplace_back(
        pngine::framebuffer(m_device, img.get(), render_pass_pos->second.get(), m_swapchain.get_extent()));
  });
}

const pngine::shader_module& device_impl::do_get_shader_module(std::string_view name) const {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно вызвать get_shader_module, пока не создан device!");
  const auto shader_module_pos = m_shader_modules.find(name.data());
  [[unlikely]] if (shader_module_pos == m_shader_modules.cend())
    throw std::runtime_error("Device: не удалось найти указанный шейдер-модуль!");
  return shader_module_pos->second;
}

const pngine::swapchainKHR& device_impl::do_get_swapchain() const {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно вызвать get_swapchain, пока не создан device!");
  return m_swapchain;
}
const pngine::render_pass& device_impl::do_get_render_pass(std::string_view name) const {
  [[unlikely]] if (m_is_created == false)
    throw std::runtime_error("Device: невозможно вызвать get_render_pass, пока не создан device!");
  const auto render_pass_pos = m_render_passes.find(name.data());
  [[unlikely]] if (render_pass_pos == m_render_passes.cend())
    throw std::runtime_error("Device: не удалось найти указанный проход рендера!");
  return render_pass_pos->second;
}

void device_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_pipelines.clear();
    m_pipeline_layouts.clear();
    m_framebuffers.clear();
    m_render_passes.clear();
    m_shader_modules.clear();
    m_image_views.clear();
    m_swapchain.clear();
    m_device.destroy();
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
