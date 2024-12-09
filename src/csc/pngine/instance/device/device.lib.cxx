module;
#include <utility>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <map>
#include <cstdint>
export module csc.pngine.instance.device;

import vulkan_hpp;

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
import csc.pngine.instance.device.render_pass;
import csc.pngine.instance.device.framebuffer;
import csc.pngine.instance.device.command_pool;
import csc.pngine.instance.device.fence;
import csc.pngine.instance.device.semaphore;

import csc.pngine.commons.utility.swapchain_details;
import csc.pngine.commons.utility.queue_family_indices;
import csc.pngine.commons.utility.graphics_pipeline;

export namespace csc {
namespace pngine {
using buf_and_mem = std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>;
using img_and_mem = std::pair<vk::UniqueImage, vk::UniqueDeviceMemory>;

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

class device {
 private:
  vk::Device m_device{};
  pngine::queue_family_indices m_indices{};
  const vk::PhysicalDevice* m_keep_phdevice{};
  const vk::SurfaceKHR* m_keep_surface{};

  pngine::swapchainKHR m_swapchain{};
  std::vector<pngine::image_view> m_image_views{};
  std::map<std::string, pngine::shader_module> m_shader_modules{};
  std::map<std::string, pngine::graphics_pipeline> m_pipelines{};
  std::map<std::string, pngine::render_pass> m_render_passes{};
  std::vector<pngine::framebuffer> m_framebuffers{};

  std::vector<const char*> m_enabled_extensions{};
  vk::Bool32 m_is_created = false;

 public:
  explicit device() = default;
  ~device() noexcept;
  device(device&& move) noexcept = default;
  device& operator=(device&& move) noexcept;
  explicit device(const vk::PhysicalDevice& dev, const vk::SurfaceKHR& surface);
  void clear() noexcept;
  vk::Device get() const;
  vk::Queue get_present_queue() const;
  vk::Queue get_graphics_queue() const;
  vk::Queue get_transfer_queue() const;
  vk::Queue get_compute_queue() const;
  const pngine::shader_module& get_shader_module(std::string_view name) const;
  const pngine::swapchainKHR& get_swapchain() const;
  const pngine::render_pass& get_render_pass(std::string_view name) const;
  const std::vector<pngine::framebuffer>& get_framebuffers() const;
  const pngine::graphics_pipeline& get_graphics_pipeline(std::string_view name) const;
  vk::PhysicalDevice get_physdev() const;

  void create_swapchainKHR(vk::Extent2D window_framebuffer, pngine::swapchain_dispatch dispatch);
  void create_image_views();
  void create_shader_module(std::string_view name, std::string_view compiled_filepath);
  template <pngine::c_graphics_pipeline_config Config>
  pngine::graphics_pipeline& create_pipeline(
      std::string_view pipeline_name,
      const vk::UniquePipelineLayout& layout,
      std::string_view pass_name,
      Config&& config);
  void create_render_pass(std::string_view pass_name);
  void create_framebuffers(std::string_view pass_name);

  buf_and_mem create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
  img_and_mem create_image(
      vk::Extent2D img_size,
      vk::Format format,
      vk::ImageTiling tiling,
      vk::ImageUsageFlags usage,
      vk::MemoryPropertyFlags required_props);
  pngine::command_pool create_graphics_command_pool(vk::CommandPoolCreateFlags flags);
  pngine::command_pool create_transfer_command_pool(vk::CommandPoolCreateFlags flags);
};

device::~device() noexcept {
  clear();
}
device& device::operator=(device&& move) noexcept {
  if (this == &move)
    return *this;
  clear();
  m_device = move.m_device;
  m_indices = move.m_indices;
  m_keep_surface = move.m_keep_surface;
  m_keep_phdevice = move.m_keep_phdevice;

  m_swapchain = std::move(move.m_swapchain);
  m_image_views = std::move(move.m_image_views);
  m_shader_modules = std::move(move.m_shader_modules);
  m_pipelines = std::move(move.m_pipelines);
  m_render_passes = std::move(move.m_render_passes);
  m_framebuffers = std::move(move.m_framebuffers);
  m_enabled_extensions = std::move(move.m_enabled_extensions);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}
device::device(const vk::PhysicalDevice& dev, const vk::SurfaceKHR& surface)
    : m_keep_phdevice(&dev), m_keep_surface(&surface) {
  m_indices = pngine::bring_indices_from_phys_device(dev, surface);

  m_enabled_extensions.reserve(256ul);
  m_enabled_extensions.emplace_back("VK_KHR_swapchain");

  [[unlikely]] if (!m_indices.transfer.has_value())
    throw std::runtime_error("Device: Не найдено transfer-семейство очередей для поверхности окна!");
  [[unlikely]] if (!m_indices.present.has_value())
    throw std::runtime_error("Device: Не найдено present-семейство очередей для поверхности окна!");
  [[unlikely]] if (!m_indices.graphics.has_value())
    throw std::runtime_error("Device: Не найдено graphics-семейство очередей для устройства!");

  std::vector<vk::DeviceQueueCreateInfo> q_descriptions;
  pngine::queues graphics_queues(m_indices.graphics.value(), {1.0});
  q_descriptions.emplace_back(graphics_queues.bring_info());
  // учёт поддержки разделения очередей на семейства
  if (m_indices.graphics.value() != m_indices.transfer.value()) {
    pngine::queues transfer_queues(m_indices.transfer.value(), {1.0}); // если индексы разные - то и очереди тоже
    q_descriptions.emplace_back(transfer_queues.bring_info());
  }
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

void device::create_swapchainKHR(vk::Extent2D window_framebuffer, pngine::swapchain_dispatch dispatch) {
  auto details = pngine::bring_swapchain_details_from_phys_device(*m_keep_phdevice, *m_keep_surface);
  details.window_framebuffer = window_framebuffer;
  m_swapchain = pngine::swapchainKHR(m_device, *m_keep_surface, details, m_indices, dispatch);
}

void device::create_image_views() {
  m_image_views.clear();
  m_image_views.reserve(m_swapchain.get_images().size());
  std::ranges::for_each(m_swapchain.get_images(), [&](const auto& img) {
    m_image_views.emplace_back(pngine::image_view(m_device, img, m_swapchain.get_image_format()));
  });
}
void device::create_shader_module(std::string_view name, std::string_view compiled_filepath) {
  m_shader_modules.insert(std::make_pair(name.data(), pngine::shader_module(m_device, compiled_filepath)));
}

template <pngine::c_graphics_pipeline_config Config>
pngine::graphics_pipeline& device::create_pipeline(
    std::string_view named_pipeline,
    const vk::UniquePipelineLayout& layout,
    std::string_view pass_name,
    Config&& config) {
  const auto render_pass_pos = m_render_passes.find(pass_name.data());
  [[unlikely]] if (render_pass_pos == m_render_passes.cend())
    throw std::runtime_error("Device: не удалось найти указанный проход конвейера!");

  const auto [pipeline_pos, is_success] = m_pipelines.insert(std::make_pair(
      named_pipeline.data(),
      pngine::graphics_pipeline(m_device, render_pass_pos->second.get(), layout.get(), std::forward<Config>(config))));

  [[unlikely]] if (!is_success)
    throw std::runtime_error("Device: не удалось добавить конвейер в реестр!");
  return pipeline_pos->second;
}

void device::create_render_pass(std::string_view name) {
  m_render_passes.insert(std::make_pair(name.data(), pngine::render_pass(m_device, m_swapchain.get_image_format())));
}

void device::create_framebuffers(std::string_view pass_name) {
  m_framebuffers.clear();
  const auto render_pass_pos = m_render_passes.find(pass_name.data());
  [[unlikely]] if (render_pass_pos == m_render_passes.cend())
    throw std::runtime_error("Device: не удалось найти указанный проход рендера!");

  m_framebuffers.reserve(m_image_views.size());
  std::ranges::for_each(m_image_views, [&](const auto& img) {
    m_framebuffers.emplace_back(
        pngine::framebuffer(m_device, img.get(), render_pass_pos->second.get(), m_swapchain.get_extent()));
  });
}

pngine::command_pool device::create_graphics_command_pool(vk::CommandPoolCreateFlags flags) {
  return pngine::command_pool(m_device, m_indices.graphics.value(), flags);
}
pngine::command_pool device::create_transfer_command_pool(vk::CommandPoolCreateFlags flags) {
  return pngine::command_pool(m_device, m_indices.transfer.value(), flags);
}

buf_and_mem
device::create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags required_props) {
  buf_and_mem result;
  vk::BufferCreateInfo buffer_info{};
  buffer_info.sType = vk::StructureType::eBufferCreateInfo;
  buffer_info.usage = usage;
  buffer_info.size = size;
  buffer_info.sharingMode = vk::SharingMode::eExclusive;
  result.first = m_device.createBufferUnique(buffer_info, nullptr);

  const auto spec_props = m_device.getBufferMemoryRequirements(result.first.get());
  const auto supported_props = m_keep_phdevice->getMemoryProperties();
  vk::MemoryAllocateInfo alloc_info{};
  alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
  alloc_info.allocationSize = spec_props.size;
  alloc_info.memoryTypeIndex = pngine::choose_memory_type(supported_props, spec_props, required_props);
  result.second = m_device.allocateMemoryUnique(alloc_info, nullptr);

  m_device.bindBufferMemory(result.first.get(), result.second.get(), 0u);
  return result;
}

img_and_mem device::create_image(
    vk::Extent2D img_size,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags required_props) {
  img_and_mem result;
  vk::ImageCreateInfo img_info{};
  img_info.sType = vk::StructureType::eImageCreateInfo;
  // img_info.pNext = extension_obj_info;
  // img_info.flags = spec_flags;
  img_info.imageType = vk::ImageType::e2D;
  img_info.usage = usage;
  img_info.extent = vk::Extent3D(img_size, 1u);
  img_info.tiling = tiling;
  img_info.format = format;
  img_info.samples = vk::SampleCountFlagBits::e1;
  img_info.mipLevels = 1u;
  img_info.arrayLayers = 1u;
  img_info.initialLayout = vk::ImageLayout::eUndefined;
  img_info.sharingMode = vk::SharingMode::eExclusive;
  result.first = m_device.createImageUnique(img_info, nullptr);

  const auto spec_props = m_device.getImageMemoryRequirements(result.first.get());
  const auto supported_props = m_keep_phdevice->getMemoryProperties();

  vk::MemoryAllocateInfo alloc_info{};
  alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
  alloc_info.allocationSize = spec_props.size;
  alloc_info.memoryTypeIndex = pngine::choose_memory_type(supported_props, spec_props, required_props);
  result.second = m_device.allocateMemoryUnique(alloc_info, nullptr);
  m_device.bindImageMemory(result.first.get(), result.second.get(), 0u);
  return result;
}

const pngine::shader_module& device::get_shader_module(std::string_view name) const {
  const auto shader_module_pos = m_shader_modules.find(name.data());
  [[unlikely]] if (shader_module_pos == m_shader_modules.cend())
    throw std::runtime_error("Device: не удалось найти указанный шейдер-модуль!");
  return shader_module_pos->second;
}

const pngine::swapchainKHR& device::get_swapchain() const {
  return m_swapchain;
}
const pngine::render_pass& device::get_render_pass(std::string_view name) const {
  const auto render_pass_pos = m_render_passes.find(name.data());
  [[unlikely]] if (render_pass_pos == m_render_passes.cend())
    throw std::runtime_error("Device: не удалось найти указанный проход рендера!");
  return render_pass_pos->second;
}

const pngine::graphics_pipeline& device::get_graphics_pipeline(std::string_view name) const {
  const auto pipeline_pos = m_pipelines.find(name.data());
  [[unlikely]] if (pipeline_pos == m_pipelines.cend())
    throw std::runtime_error("Device: не удалось найти указанный графический конвейер!");
  return pipeline_pos->second;
}

const std::vector<pngine::framebuffer>& device::get_framebuffers() const {
  return m_framebuffers;
}

vk::Device device::get() const {
  return m_device;
}
vk::PhysicalDevice device::get_physdev() const {
  return *m_keep_phdevice;
}

vk::Queue device::get_graphics_queue() const {
  return m_device.getQueue(m_indices.graphics.value(), 0);
}

vk::Queue device::get_transfer_queue() const {
  return m_device.getQueue(m_indices.transfer.value(), 0);
}

vk::Queue device::get_present_queue() const {
  return m_device.getQueue(m_indices.present.value(), 0);
}
vk::Queue device::get_compute_queue() const {
  return m_device.getQueue(m_indices.compute.value(), 0);
}
void device::clear() noexcept {
  if (m_is_created != false) {
    m_pipelines.clear();
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
