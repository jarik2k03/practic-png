module;
#include <cstdint>
#include <cstring>
#include <cmath>
#include <tuple>
#include <unistd.h>
#include <bits/stl_algo.h>
#include <shaders_clipping.h>
#include <shaders_scaling.h>
#include <shaders_rotating.h>
export module csc.pngine.image_manipulator;

export import :shaderdata;

import stl.vector;
import stl.array;
import stl.stdexcept;

import glm_hpp;

import csc.pngine.instance.device;

import vulkan_hpp;

export namespace csc {
namespace pngine {

struct image_manipulator_bundle {
  pngine::buf_and_mem staged;
  vk::Extent2D image_size;
};

vk::Extent2D force_clamp_image_extent(vk::Extent2D src, vk::Extent2D restrict) {
  return vk::Extent2D(std::clamp(src.width, 0u, restrict.width), std::clamp(src.height, 0u, restrict.height));
}

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
  vk::UniquePipeline m_clipping_pipeline;
  vk::UniquePipeline m_scaling_pipeline;
  vk::UniquePipeline m_rotating_pipeline;
  // uniform params;
  vk::UniqueDeviceMemory m_uniform_params_memory;
  vk::UniqueBuffer m_uniform_params;
  // uniform readonly image2D in_image;
  vk::UniqueDeviceMemory m_first_image_memory;
  vk::UniqueImage m_first_image;
  vk::UniqueImageView m_first_image_view;
  vk::Extent2D m_first_image_size;
  // gpu restrict;
  vk::Extent2D m_max_image_size;
  static constexpr auto m_uniform_params_max_size = 128u;

 public:
  image_manipulator() = default;
  image_manipulator(image_manipulator&& move) noexcept = default;
  image_manipulator& operator=(image_manipulator&& move) noexcept = default;
  explicit image_manipulator(
      pngine::device& handle,
      const vk::UniqueBuffer& staged_buffer,
      vk::Extent2D src_image_size);
  pngine::image_manipulator_bundle clip_image(vk::Offset2D offset, vk::Extent2D size);
  pngine::image_manipulator_bundle scale_image(float scaleX, float scaleY);
  pngine::image_manipulator_bundle rotate_image(float rotate_angle_radians);
  ~image_manipulator() noexcept = default;
};

image_manipulator::image_manipulator(
    pngine::device& handle,
    const vk::UniqueBuffer& staged_buffer,
    vk::Extent2D src_image_size)
    : m_device(&handle), m_first_image_size(src_image_size) {
  /* check max image size for gpu */
  const auto props = m_device->get_physdev().getImageFormatProperties(
      vk::Format::eR8G8B8A8Unorm,
      vk::ImageType::e2D,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc,
      vk::ImageCreateFlags{});
  m_max_image_size = vk::Extent2D(props.maxExtent.width, props.maxExtent.height);
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
      m_uniform_params_max_size, // зафиксирован максимальный размер параметров
      vk::BufferUsageFlagBits::eUniformBuffer,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  /* allocating input image storage memory */
  std::tie(m_first_image, m_first_image_memory) = m_device->create_image(
      m_first_image_size,
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

  /* compute pipelines */
  m_device->create_shader_module("cs_clipping", pngine::shaders::shaders_clipping::comp_path);
  m_device->create_shader_module("cs_scaling", pngine::shaders::shaders_scaling::comp_path);
  m_device->create_shader_module("cs_rotating", pngine::shaders::shaders_rotating::comp_path);

  vk::PipelineShaderStageCreateInfo clipping_stage_info{};
  clipping_stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  clipping_stage_info.stage = vk::ShaderStageFlagBits::eCompute;
  clipping_stage_info.module = m_device->get_shader_module("cs_clipping").get();
  clipping_stage_info.pName = "main";

  vk::PipelineShaderStageCreateInfo scaling_stage_info{};
  scaling_stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  scaling_stage_info.stage = vk::ShaderStageFlagBits::eCompute;
  scaling_stage_info.module = m_device->get_shader_module("cs_scaling").get();
  scaling_stage_info.pName = "main";

  vk::PipelineShaderStageCreateInfo rotating_stage_info{};
  rotating_stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  rotating_stage_info.stage = vk::ShaderStageFlagBits::eCompute;
  rotating_stage_info.module = m_device->get_shader_module("cs_rotating").get();
  rotating_stage_info.pName = "main";
  /* . . . */
  vk::ComputePipelineCreateInfo clipping_pipeline_info{};
  clipping_pipeline_info.sType = vk::StructureType::eComputePipelineCreateInfo;
  clipping_pipeline_info.layout = m_compute_pipeline_layout.get();
  clipping_pipeline_info.stage = clipping_stage_info;

  m_clipping_pipeline =
      m_device->get().createComputePipelineUnique(vk::PipelineCache{}, clipping_pipeline_info, nullptr).value;

  vk::ComputePipelineCreateInfo scaling_pipeline_info{};
  scaling_pipeline_info.sType = vk::StructureType::eComputePipelineCreateInfo;
  scaling_pipeline_info.layout = m_compute_pipeline_layout.get();
  scaling_pipeline_info.stage = scaling_stage_info;

  m_scaling_pipeline =
      m_device->get().createComputePipelineUnique(vk::PipelineCache{}, scaling_pipeline_info, nullptr).value;

  vk::ComputePipelineCreateInfo rotating_pipeline_info{};
  rotating_pipeline_info.sType = vk::StructureType::eComputePipelineCreateInfo;
  rotating_pipeline_info.layout = m_compute_pipeline_layout.get();
  rotating_pipeline_info.stage = rotating_stage_info;

  m_rotating_pipeline =
      m_device->get().createComputePipelineUnique(vk::PipelineCache{}, rotating_pipeline_info, nullptr).value;

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
  copy_img_region.imageExtent = vk::Extent3D(m_first_image_size, 1u);
  transfer_buffer.copyBufferToImage(
      staged_buffer.get(), m_first_image.get(), vk::ImageLayout::eTransferDstOptimal, 1u, &copy_img_region);

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
      stg::eTransfer,
      stg::eComputeShader,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &transferDst_to_general);
  transfer_buffer.end();

  /* write this to transfer queue */
  vk::SubmitInfo queue_submit_info{};
  queue_submit_info.sType = vk::StructureType::eSubmitInfo;
  queue_submit_info.pCommandBuffers = &transfer_buffer;
  queue_submit_info.commandBufferCount = 1u;
  const auto transfer_queue = m_device->get_transfer_queue();
  const vk::Result r = transfer_queue.submit(1u, &queue_submit_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось записать входное изображение в очередь передачи!");
  transfer_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);
}

pngine::image_manipulator_bundle image_manipulator::rotate_image(float rotate_angle_radians) {
  if (m_device == nullptr) // проверка на валидность класса
    throw std::runtime_error("image_manipulator:: объект не проининициализирован. Требуется валидный объект!");

  const float cos_angle = ::cosf(rotate_angle_radians), sin_angle = ::sinf(rotate_angle_radians);
  vk::Extent2D second_image_size;
  second_image_size.width =
      std::floorf(std::abs(m_first_image_size.width * cos_angle) + std::abs(m_first_image_size.height * sin_angle));
  second_image_size.height =
      std::floorf(std::abs(m_first_image_size.width * sin_angle) + std::abs(m_first_image_size.height * cos_angle));
  second_image_size = pngine::force_clamp_image_extent(second_image_size, m_max_image_size);
  /* uniform buffer with params */
  void* const mapped = m_device->get().mapMemory(m_uniform_params_memory.get(), 0u, sizeof(pngine::rotating::params));
  pngine::rotating::params params_host_buffer;
  params_host_buffer.in_middle_idx = glm::vec2(m_first_image_size.width / 2.f, m_first_image_size.height / 2.f);
  params_host_buffer.out_middle_idx = glm::vec2(second_image_size.width / 2.f, second_image_size.height / 2.f);
  params_host_buffer.cos_angle = cos_angle;
  params_host_buffer.sin_angle = sin_angle;

  ::memcpy(mapped, &params_host_buffer, sizeof(pngine::rotating::params));
  m_device->get().unmapMemory(m_uniform_params_memory.get());

  /* allocating output image storage memory */
  auto [second_image, second_image_memory] = m_device->create_image(
      second_image_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc, // запись данных будет в шейдере
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

  auto rotate_descr_set = std::move(m_device->get().allocateDescriptorSetsUnique(alloc_sets_info).at(0u));

  vk::DescriptorBufferInfo d_params_info(m_uniform_params.get(), 0u, sizeof(pngine::rotating::params));
  vk::DescriptorImageInfo d_in_image_info({}, m_first_image_view.get(), vk::ImageLayout::eGeneral);
  vk::DescriptorImageInfo d_out_image_info({}, second_image_view.get(), vk::ImageLayout::eGeneral);

  vk::WriteDescriptorSet write_params{}, write_in_image{}, write_out_image{};
  write_params.sType = vk::StructureType::eWriteDescriptorSet;
  write_params.descriptorCount = 1u;
  write_params.pBufferInfo = &d_params_info;
  write_params.descriptorType = vk::DescriptorType::eUniformBuffer;
  write_params.dstSet = rotate_descr_set.get();
  write_params.dstBinding = 0u;
  write_params.dstArrayElement = 0u;

  write_in_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_in_image.descriptorCount = 1u;
  write_in_image.pImageInfo = &d_in_image_info;
  write_in_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_in_image.dstSet = rotate_descr_set.get();
  write_in_image.dstBinding = 1u;
  write_in_image.dstArrayElement = 0u;

  write_out_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_out_image.descriptorCount = 1u;
  write_out_image.pImageInfo = &d_out_image_info;
  write_out_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_out_image.dstSet = rotate_descr_set.get();
  write_out_image.dstBinding = 2u;
  write_out_image.dstArrayElement = 0u;
  const auto writings = std::array{write_params, write_in_image, write_out_image};
  m_device->get().updateDescriptorSets(writings.size(), writings.data(), 0u, nullptr);

  /* запись команд */
  vk::CommandBufferAllocateInfo comp_alloc_info{};
  comp_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  comp_alloc_info.commandPool = m_compute_pool.get();
  comp_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  comp_alloc_info.commandBufferCount = 1u;
  auto compute_buffers = m_device->get().allocateCommandBuffers(comp_alloc_info);
  auto compute_buffer = compute_buffers[0u];

  vk::CommandBufferBeginInfo compute_begin_info{};
  compute_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  compute_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  compute_buffer.begin(compute_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier undef_to_general_2{};
  undef_to_general_2.sType = vk::StructureType::eImageMemoryBarrier;
  undef_to_general_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.image = second_image.get();
  undef_to_general_2.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  undef_to_general_2.oldLayout = vk::ImageLayout::eUndefined;
  undef_to_general_2.newLayout = vk::ImageLayout::eGeneral;
  undef_to_general_2.srcAccessMask = vk::AccessFlags{};
  undef_to_general_2.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
  using stg = vk::PipelineStageFlagBits;
  compute_buffer.pipelineBarrier(
      stg::eTopOfPipe, stg::eComputeShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &undef_to_general_2);
  /* bindPipeline */
  compute_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_rotating_pipeline.get());
  /* bindDescriptorSets */
  compute_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eCompute, m_compute_pipeline_layout.get(), 0u, 1u, &rotate_descr_set.get(), 0u, nullptr);
  /* dispatch */
  compute_buffer.dispatch(
      std::roundf(second_image_size.width / 16.f), std::roundf(second_image_size.height / 16.f), 1u);

  compute_buffer.end();

  vk::SubmitInfo submit_comp_info{};
  submit_comp_info.sType = vk::StructureType::eSubmitInfo;
  submit_comp_info.pCommandBuffers = &compute_buffer;
  submit_comp_info.commandBufferCount = 1u;

  const auto compute_queue = m_device->get_compute_queue();
  vk::Result r = compute_queue.submit(1u, &submit_comp_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось сделать запись в вычислительную очередь успешно!");
  compute_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_compute_pool.get(), 1u, &compute_buffer);

  /* создание промежуточного буфера для копирования изображения */
  pngine::image_manipulator_bundle staged;
  staged.staged = m_device->create_buffer(
      second_image_size.width * second_image_size.height * 4u,
      vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  staged.image_size = second_image_size;

  /* command buffer for transfer */
  vk::CommandBufferAllocateInfo transfer_alloc_info{};
  transfer_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  transfer_alloc_info.commandPool = m_transfer_pool.get();
  transfer_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  transfer_alloc_info.commandBufferCount = 1u;

  auto transfer_buffers = m_device->get().allocateCommandBuffers(transfer_alloc_info);
  auto& transfer_buffer = transfer_buffers[0u];

  vk::CommandBufferBeginInfo transfer_begin_info{};
  transfer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  transfer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  transfer_buffer.begin(transfer_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier general_to_transferSrc_2{};
  general_to_transferSrc_2.sType = vk::StructureType::eImageMemoryBarrier;
  general_to_transferSrc_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_transferSrc_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_transferSrc_2.image = second_image.get();
  general_to_transferSrc_2.subresourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  general_to_transferSrc_2.oldLayout = vk::ImageLayout::eGeneral;
  general_to_transferSrc_2.newLayout = vk::ImageLayout::eTransferSrcOptimal;
  general_to_transferSrc_2.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
  general_to_transferSrc_2.dstAccessMask = vk::AccessFlagBits::eTransferRead;
  transfer_buffer.pipelineBarrier(
      stg::eComputeShader,
      stg::eTransfer,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &general_to_transferSrc_2);
  /* copyImageToBuffer */
  vk::BufferImageCopy copy_img_region{};
  copy_img_region.bufferOffset = 0u;
  copy_img_region.bufferRowLength = 0u;
  copy_img_region.bufferImageHeight = 0u;
  copy_img_region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u);
  copy_img_region.imageOffset = vk::Offset3D(0u, 0u, 0u);
  copy_img_region.imageExtent = vk::Extent3D(second_image_size, 1u);
  transfer_buffer.copyImageToBuffer(
      second_image.get(), vk::ImageLayout::eTransferSrcOptimal, staged.staged.first.get(), 1u, &copy_img_region);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier transferSrc_2_to_general{};
  transferSrc_2_to_general.sType = vk::StructureType::eImageMemoryBarrier;
  transferSrc_2_to_general.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferSrc_2_to_general.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferSrc_2_to_general.image = second_image.get();
  transferSrc_2_to_general.subresourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  transferSrc_2_to_general.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
  transferSrc_2_to_general.newLayout = vk::ImageLayout::eGeneral;
  transferSrc_2_to_general.srcAccessMask = vk::AccessFlagBits::eTransferRead;
  transferSrc_2_to_general.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  transfer_buffer.pipelineBarrier(
      stg::eTransfer,
      stg::eComputeShader,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &transferSrc_2_to_general);
  transfer_buffer.end();

  vk::SubmitInfo submit_transfer_info{};
  submit_transfer_info.sType = vk::StructureType::eSubmitInfo;
  submit_transfer_info.pCommandBuffers = &transfer_buffer;
  submit_transfer_info.commandBufferCount = 1u;
  const auto transfer_queue = m_device->get_transfer_queue();
  r = transfer_queue.submit(1u, &submit_transfer_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось скопировать изображение в промежуточный буфер!");
  transfer_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_size = second_image_size, m_first_image_memory = std::move(second_image_memory);

  return staged;
}

pngine::image_manipulator_bundle image_manipulator::scale_image(float scaleX, float scaleY) {
  if (m_device == nullptr) // проверка на валидность класса
    throw std::runtime_error("image_manipulator:: объект не проининициализирован. Требуется валидный объект!");
  /* uniform buffer with params */
  void* const mapped = m_device->get().mapMemory(m_uniform_params_memory.get(), 0u, sizeof(pngine::scaling::params));
  const pngine::scaling::params params_host_buffer{glm::vec2(scaleX, scaleY)};
  ::memcpy(mapped, &params_host_buffer, sizeof(pngine::scaling::params));
  m_device->get().unmapMemory(m_uniform_params_memory.get());

  /* allocating output image storage memory */
  const auto second_image_size =
      pngine::force_clamp_image_extent(vk::Extent2D(std::roundf(m_first_image_size.width * scaleX), std::roundf(m_first_image_size.height * scaleY)), m_max_image_size);
  auto [second_image, second_image_memory] = m_device->create_image(
      second_image_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc, // запись данных будет в шейдере
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

  auto scale_descr_set = std::move(m_device->get().allocateDescriptorSetsUnique(alloc_sets_info).at(0u));

  vk::DescriptorBufferInfo d_params_info(m_uniform_params.get(), 0u, sizeof(pngine::scaling::params));
  vk::DescriptorImageInfo d_in_image_info({}, m_first_image_view.get(), vk::ImageLayout::eGeneral);
  vk::DescriptorImageInfo d_out_image_info({}, second_image_view.get(), vk::ImageLayout::eGeneral);

  vk::WriteDescriptorSet write_params{}, write_in_image{}, write_out_image{};
  write_params.sType = vk::StructureType::eWriteDescriptorSet;
  write_params.descriptorCount = 1u;
  write_params.pBufferInfo = &d_params_info;
  write_params.descriptorType = vk::DescriptorType::eUniformBuffer;
  write_params.dstSet = scale_descr_set.get();
  write_params.dstBinding = 0u;
  write_params.dstArrayElement = 0u;

  write_in_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_in_image.descriptorCount = 1u;
  write_in_image.pImageInfo = &d_in_image_info;
  write_in_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_in_image.dstSet = scale_descr_set.get();
  write_in_image.dstBinding = 1u;
  write_in_image.dstArrayElement = 0u;

  write_out_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_out_image.descriptorCount = 1u;
  write_out_image.pImageInfo = &d_out_image_info;
  write_out_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_out_image.dstSet = scale_descr_set.get();
  write_out_image.dstBinding = 2u;
  write_out_image.dstArrayElement = 0u;
  const auto writings = std::array{write_params, write_in_image, write_out_image};
  m_device->get().updateDescriptorSets(writings.size(), writings.data(), 0u, nullptr);

  /* запись команд */
  vk::CommandBufferAllocateInfo comp_alloc_info{};
  comp_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  comp_alloc_info.commandPool = m_compute_pool.get();
  comp_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  comp_alloc_info.commandBufferCount = 1u;
  auto compute_buffers = m_device->get().allocateCommandBuffers(comp_alloc_info);
  auto compute_buffer = compute_buffers[0u];

  vk::CommandBufferBeginInfo compute_begin_info{};
  compute_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  compute_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  compute_buffer.begin(compute_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier undef_to_general_2{};
  undef_to_general_2.sType = vk::StructureType::eImageMemoryBarrier;
  undef_to_general_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.image = second_image.get();
  undef_to_general_2.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  undef_to_general_2.oldLayout = vk::ImageLayout::eUndefined;
  undef_to_general_2.newLayout = vk::ImageLayout::eGeneral;
  undef_to_general_2.srcAccessMask = vk::AccessFlags{};
  undef_to_general_2.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
  using stg = vk::PipelineStageFlagBits;
  compute_buffer.pipelineBarrier(
      stg::eTopOfPipe, stg::eComputeShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &undef_to_general_2);
  /* bindPipeline */
  compute_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_scaling_pipeline.get());
  /* bindDescriptorSets */
  compute_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eCompute, m_compute_pipeline_layout.get(), 0u, 1u, &scale_descr_set.get(), 0u, nullptr);
  /* dispatch */
  compute_buffer.dispatch(std::ceilf(second_image_size.width / 16.f), std::ceilf(second_image_size.height / 16.f), 1u);

  compute_buffer.end();

  vk::SubmitInfo submit_comp_info{};
  submit_comp_info.sType = vk::StructureType::eSubmitInfo;
  submit_comp_info.pCommandBuffers = &compute_buffer;
  submit_comp_info.commandBufferCount = 1u;

  const auto compute_queue = m_device->get_compute_queue();
  vk::Result r = compute_queue.submit(1u, &submit_comp_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось сделать запись в вычислительную очередь успешно!");
  compute_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_compute_pool.get(), 1u, &compute_buffer);

  /* создание промежуточного буфера для копирования изображения */
  pngine::image_manipulator_bundle staged;
  staged.staged = m_device->create_buffer(
      second_image_size.width * second_image_size.height * 4u,
      vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  staged.image_size = second_image_size;

  /* command buffer for transfer */
  vk::CommandBufferAllocateInfo transfer_alloc_info{};
  transfer_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  transfer_alloc_info.commandPool = m_transfer_pool.get();
  transfer_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  transfer_alloc_info.commandBufferCount = 1u;

  auto transfer_buffers = m_device->get().allocateCommandBuffers(transfer_alloc_info);
  auto& transfer_buffer = transfer_buffers[0u];

  vk::CommandBufferBeginInfo transfer_begin_info{};
  transfer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  transfer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  transfer_buffer.begin(transfer_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier general_to_transferSrc_2{};
  general_to_transferSrc_2.sType = vk::StructureType::eImageMemoryBarrier;
  general_to_transferSrc_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_transferSrc_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_transferSrc_2.image = second_image.get();
  general_to_transferSrc_2.subresourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  general_to_transferSrc_2.oldLayout = vk::ImageLayout::eGeneral;
  general_to_transferSrc_2.newLayout = vk::ImageLayout::eTransferSrcOptimal;
  general_to_transferSrc_2.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
  general_to_transferSrc_2.dstAccessMask = vk::AccessFlagBits::eTransferRead;
  transfer_buffer.pipelineBarrier(
      stg::eComputeShader,
      stg::eTransfer,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &general_to_transferSrc_2);
  /* copyImageToBuffer */
  vk::BufferImageCopy copy_img_region{};
  copy_img_region.bufferOffset = 0u;
  copy_img_region.bufferRowLength = 0u;
  copy_img_region.bufferImageHeight = 0u;
  copy_img_region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u);
  copy_img_region.imageOffset = vk::Offset3D(0u, 0u, 0u);
  copy_img_region.imageExtent = vk::Extent3D(second_image_size, 1u);
  transfer_buffer.copyImageToBuffer(
      second_image.get(), vk::ImageLayout::eTransferSrcOptimal, staged.staged.first.get(), 1u, &copy_img_region);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier transferSrc_2_to_general{};
  transferSrc_2_to_general.sType = vk::StructureType::eImageMemoryBarrier;
  transferSrc_2_to_general.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferSrc_2_to_general.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferSrc_2_to_general.image = second_image.get();
  transferSrc_2_to_general.subresourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  transferSrc_2_to_general.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
  transferSrc_2_to_general.newLayout = vk::ImageLayout::eGeneral;
  transferSrc_2_to_general.srcAccessMask = vk::AccessFlagBits::eTransferRead;
  transferSrc_2_to_general.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  transfer_buffer.pipelineBarrier(
      stg::eTransfer,
      stg::eComputeShader,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &transferSrc_2_to_general);
  transfer_buffer.end();

  vk::SubmitInfo submit_transfer_info{};
  submit_transfer_info.sType = vk::StructureType::eSubmitInfo;
  submit_transfer_info.pCommandBuffers = &transfer_buffer;
  submit_transfer_info.commandBufferCount = 1u;
  const auto transfer_queue = m_device->get_transfer_queue();
  r = transfer_queue.submit(1u, &submit_transfer_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось скопировать изображение в промежуточный буфер!");
  transfer_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_size = second_image_size, m_first_image_memory = std::move(second_image_memory);

  return staged;
}

pngine::image_manipulator_bundle image_manipulator::clip_image(vk::Offset2D offset, vk::Extent2D size) {
  if (m_device == nullptr) // проверка на валидность класса
    throw std::runtime_error("image_manipulator:: объект не проининициализирован. Требуется валидный объект!");
  /* uniform buffer with params */
  void* const mapped = m_device->get().mapMemory(m_uniform_params_memory.get(), 0u, sizeof(pngine::clipping::params));
  const pngine::clipping::params params_host_buffer{glm::ivec2(offset.x, offset.y)};
  ::memcpy(mapped, &params_host_buffer, sizeof(pngine::clipping::params));
  m_device->get().unmapMemory(m_uniform_params_memory.get());

  /* allocating output image storage memory */
  const auto second_image_size = pngine::force_clamp_image_extent(
      vk::Extent2D(size.width - offset.x, size.height - offset.y),
      m_first_image_size); // ограничение обрезки под исходное изображение
  auto [second_image, second_image_memory] = m_device->create_image(
      second_image_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc, // запись данных будет в шейдере
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

  /* запись команд */
  vk::CommandBufferAllocateInfo comp_alloc_info{};
  comp_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  comp_alloc_info.commandPool = m_compute_pool.get();
  comp_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  comp_alloc_info.commandBufferCount = 1u;
  auto compute_buffers = m_device->get().allocateCommandBuffers(comp_alloc_info);
  auto compute_buffer = compute_buffers[0u];

  vk::CommandBufferBeginInfo compute_begin_info{};
  compute_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  compute_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  compute_buffer.begin(compute_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier undef_to_general_2{};
  undef_to_general_2.sType = vk::StructureType::eImageMemoryBarrier;
  undef_to_general_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.image = second_image.get();
  undef_to_general_2.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  undef_to_general_2.oldLayout = vk::ImageLayout::eUndefined;
  undef_to_general_2.newLayout = vk::ImageLayout::eGeneral;
  undef_to_general_2.srcAccessMask = vk::AccessFlags{};
  undef_to_general_2.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
  using stg = vk::PipelineStageFlagBits;
  compute_buffer.pipelineBarrier(
      stg::eTopOfPipe, stg::eComputeShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &undef_to_general_2);
  /* bindPipeline */
  compute_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_clipping_pipeline.get());
  /* bindDescriptorSets */
  compute_buffer.bindDescriptorSets(
      vk::PipelineBindPoint::eCompute, m_compute_pipeline_layout.get(), 0u, 1u, &clip_descr_set.get(), 0u, nullptr);
  /* dispatch */
  compute_buffer.dispatch(std::ceilf(second_image_size.width / 16.f), std::ceilf(second_image_size.height / 16.f), 1u);

  compute_buffer.end();

  vk::SubmitInfo submit_comp_info{};
  submit_comp_info.sType = vk::StructureType::eSubmitInfo;
  submit_comp_info.pCommandBuffers = &compute_buffer;
  submit_comp_info.commandBufferCount = 1u;

  const auto compute_queue = m_device->get_compute_queue();
  vk::Result r = compute_queue.submit(1u, &submit_comp_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось сделать запись в вычислительную очередь успешно!");
  compute_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_compute_pool.get(), 1u, &compute_buffer);

  /* создание промежуточного буфера для копирования изображения */
  pngine::image_manipulator_bundle staged;
  staged.staged = m_device->create_buffer(
      second_image_size.width * second_image_size.height * 4u,
      vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  staged.image_size = second_image_size;

  /* command buffer for transfer */
  vk::CommandBufferAllocateInfo transfer_alloc_info{};
  transfer_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  transfer_alloc_info.commandPool = m_transfer_pool.get();
  transfer_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  transfer_alloc_info.commandBufferCount = 1u;

  auto transfer_buffers = m_device->get().allocateCommandBuffers(transfer_alloc_info);
  auto& transfer_buffer = transfer_buffers[0u];

  vk::CommandBufferBeginInfo transfer_begin_info{};
  transfer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  transfer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  transfer_buffer.begin(transfer_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier general_to_transferSrc_2{};
  general_to_transferSrc_2.sType = vk::StructureType::eImageMemoryBarrier;
  general_to_transferSrc_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_transferSrc_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_transferSrc_2.image = second_image.get();
  general_to_transferSrc_2.subresourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  general_to_transferSrc_2.oldLayout = vk::ImageLayout::eGeneral;
  general_to_transferSrc_2.newLayout = vk::ImageLayout::eTransferSrcOptimal;
  general_to_transferSrc_2.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
  general_to_transferSrc_2.dstAccessMask = vk::AccessFlagBits::eTransferRead;
  transfer_buffer.pipelineBarrier(
      stg::eComputeShader,
      stg::eTransfer,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &general_to_transferSrc_2);
  /* copyImageToBuffer */
  vk::BufferImageCopy copy_img_region{};
  copy_img_region.bufferOffset = 0u;
  copy_img_region.bufferRowLength = 0u;
  copy_img_region.bufferImageHeight = 0u;
  copy_img_region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u);
  copy_img_region.imageOffset = vk::Offset3D(0u, 0u, 0u);
  copy_img_region.imageExtent = vk::Extent3D(second_image_size, 1u);
  transfer_buffer.copyImageToBuffer(
      second_image.get(), vk::ImageLayout::eTransferSrcOptimal, staged.staged.first.get(), 1u, &copy_img_region);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier transferSrc_2_to_general{};
  transferSrc_2_to_general.sType = vk::StructureType::eImageMemoryBarrier;
  transferSrc_2_to_general.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferSrc_2_to_general.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferSrc_2_to_general.image = second_image.get();
  transferSrc_2_to_general.subresourceRange =
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  transferSrc_2_to_general.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
  transferSrc_2_to_general.newLayout = vk::ImageLayout::eGeneral;
  transferSrc_2_to_general.srcAccessMask = vk::AccessFlagBits::eTransferRead;
  transferSrc_2_to_general.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  transfer_buffer.pipelineBarrier(
      stg::eTransfer,
      stg::eComputeShader,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &transferSrc_2_to_general);
  transfer_buffer.end();

  vk::SubmitInfo submit_transfer_info{};
  submit_transfer_info.sType = vk::StructureType::eSubmitInfo;
  submit_transfer_info.pCommandBuffers = &transfer_buffer;
  submit_transfer_info.commandBufferCount = 1u;
  const auto transfer_queue = m_device->get_transfer_queue();
  r = transfer_queue.submit(1u, &submit_transfer_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось скопировать изображение в промежуточный буфер!");
  transfer_queue.waitIdle();
  m_device->get().freeCommandBuffers(m_transfer_pool.get(), 1u, &transfer_buffer);

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_size = second_image_size, m_first_image_memory = std::move(second_image_memory);
  return staged;
}

} // namespace pngine
} // namespace csc
