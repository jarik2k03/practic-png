#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_xcb.h>
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
}
