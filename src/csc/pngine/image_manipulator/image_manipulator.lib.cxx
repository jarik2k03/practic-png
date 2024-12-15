module;
#include <cstdint>
#include <cstring>
#include <tuple>
#include <glm/glm.hpp>
#include <shaders_clipping.h>
export module csc.pngine.image_manipulator;

import :shaderdata;

import stl.vector;
import stl.array;
import stl.stdexcept;

import csc.pngine.instance.device;

import vulkan_hpp;

export namespace csc {
namespace pngine {
class image_manipulator {
 private:
  pngine::device* m_device = nullptr;

  vk::UniqueDescriptorPool m_descriptor_pool;
  vk::UniqueCommandPool m_compute_pool;
  vk::UniqueCommandPool m_transfer_pool;
  // layouts;
  vk::UniqueDescriptorSetLayout m_descr_set_layout;
  vk::UniquePipelineLayout m_compute_pipeline_layout;
  // pipeline;
  vk::UniquePipeline m_compute_pipeline;
  // uniform params;
  vk::UniqueDeviceMemory m_uniform_params_memory;
  vk::UniqueBuffer m_uniform_params;
  // uniform readonly image2D in_image;
  vk::UniqueDeviceMemory m_first_image_memory;
  vk::UniqueImage m_first_image;
  vk::UniqueImageView m_first_image_view;

 public:
  image_manipulator() = default;
  image_manipulator(image_manipulator&& move) noexcept = default;
  image_manipulator& operator=(image_manipulator&& move) noexcept = default;
  explicit image_manipulator(
      pngine::device& handle,
      const vk::UniqueBuffer& staged_buffer,
      vk::Extent2D src_image_size);
  void clip_image(vk::Offset2D offset, vk::Extent2D size);
  ~image_manipulator() noexcept = default;
};

image_manipulator::image_manipulator(
    pngine::device& handle,
    const vk::UniqueBuffer& staged_buffer,
    vk::Extent2D src_image_size)
    : m_device(&handle) {
  /* descriptor layouts */
  vk::DescriptorSetLayoutBinding u0_bind(
      0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eCompute, nullptr);
  vk::DescriptorSetLayoutBinding rdimg_u1_bind(
      1u, vk::DescriptorType::eStorageImage, 1u, vk::ShaderStageFlagBits::eCompute, nullptr);
  vk::DescriptorSetLayoutBinding wrimg_u2_bind(
      2u, vk::DescriptorType::eStorageImage, 1u, vk::ShaderStageFlagBits::eCompute, nullptr);

  const auto binds = std::array{u0_bind, rdimg_u1_bind, wrimg_u2_bind}; // deduction guides!

  vk::DescriptorSetLayoutCreateInfo descriptor_layout_info{};
  descriptor_layout_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
  descriptor_layout_info.bindingCount = binds.size();
  descriptor_layout_info.pBindings = binds.data();

  m_descr_set_layout = m_device->get().createDescriptorSetLayoutUnique(descriptor_layout_info, nullptr);

  /* compute pipeline layout */
  vk::PipelineLayoutCreateInfo compute_pipeline_layout_info{};
  compute_pipeline_layout_info.pSetLayouts = &m_descr_set_layout.get();
  compute_pipeline_layout_info.setLayoutCount = 1u;

  m_compute_pipeline_layout = m_device->get().createPipelineLayoutUnique(compute_pipeline_layout_info, nullptr);

  /* descriptor pool for toolbox */
  vk::DescriptorPoolSize params_pool_size(vk::DescriptorType::eUniformBuffer, 1u);
  vk::DescriptorPoolSize images_pool_size(vk::DescriptorType::eStorageImage, 2u);
  const auto tbx_pool_sizes = std::array{params_pool_size, images_pool_size};

  vk::DescriptorPoolCreateInfo des_pool_toolbox_info{};
  des_pool_toolbox_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
  des_pool_toolbox_info.pPoolSizes = tbx_pool_sizes.data();
  des_pool_toolbox_info.poolSizeCount = tbx_pool_sizes.size();
  des_pool_toolbox_info.maxSets = 10u; // гвоздь 10 см
  des_pool_toolbox_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

  m_descriptor_pool = m_device->get().createDescriptorPoolUnique(des_pool_toolbox_info, nullptr);

  /* allocating an empty uniform buffer */
  std::tie(m_uniform_params, m_uniform_params_memory) = m_device->create_buffer(
      128u, // зафиксирован максимальный размер параметров
      vk::BufferUsageFlagBits::eUniformBuffer,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  /* allocating input image storage memory */
  std::tie(m_first_image, m_first_image_memory) = m_device->create_image(
      src_image_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  /* image view for input image */
  vk::ImageViewCreateInfo in_image_view_info{};
  in_image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  in_image_view_info.image = m_first_image.get();
  in_image_view_info.format = vk::Format::eR8G8B8A8Unorm;
  in_image_view_info.viewType = vk::ImageViewType::e2D;
  in_image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  m_first_image_view = m_device->get().createImageViewUnique(in_image_view_info, nullptr);

  /* creating compute and transfer command pools */
  m_compute_pool = m_device->create_compute_command_pool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
  m_transfer_pool = m_device->create_transfer_command_pool(vk::CommandPoolCreateFlagBits::eTransient);

  /* compute pipeline */
  m_device->create_shader_module("cs_clipping", pngine::shaders::shaders_clipping::comp_path);

  vk::PipelineShaderStageCreateInfo clipping_stage_info{};
  clipping_stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  clipping_stage_info.stage = vk::ShaderStageFlagBits::eCompute;
  clipping_stage_info.module = m_device->get_shader_module("cs_clipping").get();
  clipping_stage_info.pName = "main";

  vk::ComputePipelineCreateInfo pipeline_info{};
  pipeline_info.sType = vk::StructureType::eComputePipelineCreateInfo;
  pipeline_info.layout = m_compute_pipeline_layout.get();
  pipeline_info.stage = clipping_stage_info;

  m_compute_pipeline = m_device->get().createComputePipelineUnique(vk::PipelineCache{}, pipeline_info, nullptr).value;

  /* command buffer for transfer */
  vk::CommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  alloc_info.commandPool = m_transfer_pool.get();
  alloc_info.level = vk::CommandBufferLevel::ePrimary;
  alloc_info.commandBufferCount = 1u;

  auto transfer_buffers = m_device->get().allocateCommandBuffers(alloc_info);
  auto& transfer_buffer = transfer_buffers[0u];

  vk::CommandBufferBeginInfo transfer_begin_info{};
  transfer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  transfer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  transfer_buffer.begin(transfer_begin_info);

  /* pipelineBarrier */
  vk::ImageMemoryBarrier undef_to_transferDst{};
  undef_to_transferDst.sType = vk::StructureType::eImageMemoryBarrier;
  undef_to_transferDst.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_transferDst.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_transferDst.image = m_first_image.get();
  undef_to_transferDst.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  undef_to_transferDst.oldLayout = vk::ImageLayout::eUndefined;
  undef_to_transferDst.newLayout = vk::ImageLayout::eTransferDstOptimal;
  undef_to_transferDst.srcAccessMask = vk::AccessFlags{};
  undef_to_transferDst.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
  using stg = vk::PipelineStageFlagBits;
  transfer_buffer.pipelineBarrier(
      stg::eTopOfPipe, stg::eTransfer, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &undef_to_transferDst);

  /* copyBufferToImage */
  vk::BufferImageCopy copy_img_region{};
  copy_img_region.bufferOffset = 0u;
  copy_img_region.bufferRowLength = 0u;
  copy_img_region.bufferImageHeight = 0u;
  copy_img_region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u);
  copy_img_region.imageOffset = vk::Offset3D(0, 0, 0);
  copy_img_region.imageExtent = vk::Extent3D(src_image_size, 1u);
  transfer_buffer.copyBufferToImage(staged_buffer.get(), m_first_image.get(), vk::ImageLayout::eTransferDstOptimal, 1u, &copy_img_region);

  /* pipelineBarrier */
  vk::ImageMemoryBarrier transferDst_to_general{};
  transferDst_to_general.sType = vk::StructureType::eImageMemoryBarrier;
  transferDst_to_general.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferDst_to_general.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferDst_to_general.image = m_first_image.get();
  transferDst_to_general.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  transferDst_to_general.oldLayout = vk::ImageLayout::eTransferDstOptimal;
  transferDst_to_general.newLayout = vk::ImageLayout::eGeneral;
  transferDst_to_general.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  transferDst_to_general.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  using stg = vk::PipelineStageFlagBits;
  transfer_buffer.pipelineBarrier(
      stg::eTransfer, stg::eComputeShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &transferDst_to_general);
  transfer_buffer.end();

  /* write this to transfer queue */
  vk::SubmitInfo queue_submit_info{};
  queue_submit_info.sType = vk::StructureType::eSubmitInfo;
  queue_submit_info.pCommandBuffers = &transfer_buffer;
  queue_submit_info.commandBufferCount = 1u;
  const auto transfer_queue = m_device->get_compute_queue();
  const vk::Result r = transfer_queue.submit(1u, &queue_submit_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось записать входное изображение в очередь передачи!");
  transfer_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);

}

void image_manipulator::clip_image(vk::Offset2D offset, vk::Extent2D size) {
  if (m_device == nullptr) // проверка на валидность класса
    throw std::runtime_error("image_manipulator:: объект не проининициализирован. Требуется валидный объект!");
  /* uniform buffer with params */
  void* const mapped = m_device->get().mapMemory(m_uniform_params_memory.get(), 0u, sizeof(pngine::clipping::params));
  const pngine::clipping::params params_host_buffer{
      glm::uvec2(offset.x, offset.y), glm::uvec2(size.width, size.height)};
  ::memcpy(mapped, &params_host_buffer, sizeof(pngine::clipping::params));
  m_device->get().unmapMemory(m_uniform_params_memory.get());

  /* allocating output image storage memory */
  const auto clip_size = vk::Extent2D(size.width - offset.x, size.height - offset.y);
  auto [second_image, second_image_memory] = m_device->create_image(
      clip_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage, // запись данных будет в шейдере
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  /* image view for output image */
  vk::ImageViewCreateInfo out_image_view_info{};
  out_image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  out_image_view_info.image = second_image.get();
  out_image_view_info.format = vk::Format::eR8G8B8A8Unorm;
  out_image_view_info.viewType = vk::ImageViewType::e2D;
  out_image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  auto second_image_view = m_device->get().createImageViewUnique(out_image_view_info, nullptr);

  /* аллокация и запись дескрипторов */
  vk::DescriptorSetAllocateInfo alloc_sets_info{};
  alloc_sets_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
  alloc_sets_info.descriptorPool = m_descriptor_pool.get();
  alloc_sets_info.descriptorSetCount = 1u;
  alloc_sets_info.pSetLayouts = &m_descr_set_layout.get();

  auto clip_descr_set = std::move(m_device->get().allocateDescriptorSetsUnique(alloc_sets_info).at(0u));

  vk::DescriptorBufferInfo d_params_info(m_uniform_params.get(), 0u, sizeof(pngine::clipping::params));
  vk::DescriptorImageInfo d_in_image_info({}, m_first_image_view.get(), vk::ImageLayout::eGeneral);
  vk::DescriptorImageInfo d_out_image_info({}, second_image_view.get(), vk::ImageLayout::eGeneral);

  vk::WriteDescriptorSet write_params{}, write_in_image{}, write_out_image{};
  write_params.sType = vk::StructureType::eWriteDescriptorSet;
  write_params.descriptorCount = 1u;
  write_params.pBufferInfo = &d_params_info;
  write_params.descriptorType = vk::DescriptorType::eUniformBuffer;
  write_params.dstSet = clip_descr_set.get();
  write_params.dstBinding = 0u;
  write_params.dstArrayElement = 0u;

  write_in_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_in_image.descriptorCount = 1u;
  write_in_image.pImageInfo = &d_in_image_info;
  write_in_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_in_image.dstSet = clip_descr_set.get();
  write_in_image.dstBinding = 1u;
  write_in_image.dstArrayElement = 0u;

  write_out_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_out_image.descriptorCount = 1u;
  write_out_image.pImageInfo = &d_out_image_info;
  write_out_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_out_image.dstSet = clip_descr_set.get();
  write_out_image.dstBinding = 2u;
  write_out_image.dstArrayElement = 0u;
  const auto writings = std::array{write_params, write_in_image, write_out_image};
  m_device->get().updateDescriptorSets(writings.size(), writings.data(), 0u, nullptr);

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_memory = std::move(second_image_memory); // теперь выходной буфер станет входным
}

} // namespace pngine
} // namespace csc
