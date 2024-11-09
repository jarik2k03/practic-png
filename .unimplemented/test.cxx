#include <vulkan/vulkan.hpp>
int main() {
  struct dispatch_t {};
  dispatch_t dispatch;
  vk::DebugReportCallbackEXT report;
  vk::InstanceCreateInfo instinf;
  vk::Instance instance = vk::createInstance(instinf, nullptr, {});
  vk::Flags<vk::DebugReportFlagBitsEXT> flags;
  auto dev = instance.enumeratePhysicalDevices();
  dev[0].getQueueFamilyProperties().at(0).queueFlags;
  vk::DeviceCreateInfo devinf;
  vk::Device device = dev[0].createDevice(devinf);
  vk::SurfaceKHR surface;
  dev[0].getSurfaceSupportKHR(0, surface);
  const auto families = dev[0u].getQueueFamilyProperties();
  const auto caps = dev[0].getSurfaceCapabilitiesKHR(surface, dispatch);
  const auto formats = dev[0].getSurfaceFormatsKHR(surface,dispatch);
  const auto modes = dev[0].getSurfacePresentModesKHR(surface, dispatch);
  vk::SwapchainCreateInfoKHR swapchaininfo;
  vk::StructureType::eSwapchainCreateInfoKHR;
  vk::SwapchainKHR swapchain;
  const auto images = device.getSwapchainImagesKHR(swapchain);
  vk::ImageView img_view;
  device.destroyImageView(img_view, nullptr);
  vk::PipelineShaderStageCreateInfo shader_stage_info;
  auto pipeline_r = device.createGraphicsPipeline(vk::PipelineCache(), {}, nullptr);
  vk::Pipeline pipeline = pipeline_r.value;
  vk::PipelineColorBlendAttachmentState state;
  state.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
  state.destColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
  state.srcAlphaBlendFactor = vk::BlendFactor::eOne;
  state.destAlphaBlendFactor = vk::BlendFactor::eZero;
  device.acquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(), m_image_available_s);
  vk::CommandBuffer cb;
  instance.destroySurfaceKHR(surface, nullptr);
  vk::BufferCreateInfo buffer{};
  buffer.sharingMode = vk::SharingMode::eExclusive;
  buffer.usage = vk::BufferUsageFlagBits::eVertexBuffer;
  buffer.size = sizeof(pngine::vertex) * 3u; // для обычного треугольника
  vk::Buffer vertex_buffer{};
  const auto required_properties = device.getBufferMemoryRequirements(vertex_buffer);
  const auto all_properties = dev[0].getMemoryProperties();
  all_properties.memoryTypes;
  vk::CommandPool pool;
  const vk::UniqueDeviceMemory mem = device.allocateMemoryUnique({});
  void* data = device.mapMemory(m_png_surface_mesh_memory, 0u, vk::WholeSize);
  device.unmapMemory(m_png_surface_mesh_memory);
  vk::DescriptorSetLayoutBinding descr_layout_bind;
  cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0u, {m_set}, {});
  vk::PipelineLayoutCreateInfo pip_lay{};
  vk::DescriptorSetLayoutBinding lay_bind{};
  vk::ImageMemoryBarrier img_info;
  img_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  vk::ImageSubresourceLayers subres;
  vk::SubmitInfo submit(.
}
