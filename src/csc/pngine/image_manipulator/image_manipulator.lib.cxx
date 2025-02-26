module;
#include <cstdint>
#include <cstring>
#include <cmath>
#include <tuple>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <bits/stl_algo.h>
#include <shaders_clipping.h>
#include <shaders_scaling.h>
#include <shaders_rotating.h>
export module csc.pngine.image_manipulator;

import csc.pngine.commons.toolbox_params;

import stl.vector;
import stl.array;
import stl.stdexcept;

import glm_hpp;

import csc.pngine.instance.device;

import vulkan_hpp;

export namespace csc {
namespace pngine {

vk::Extent2D force_clamp_image_extent(vk::Extent2D src, vk::Extent2D restrict) {
  if (src.width == 0u && src.height == 0u)
    return vk::Extent2D(1u, 1u);
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
  // methods;
  void Update_Descriptor_Set(vk::DescriptorSet set, uint32_t uniform_size, vk::ImageView second_view);
  pngine::img_and_mem Create_Second_Image(vk::Extent2D size);
  vk::UniqueImageView Create_ImageView(vk::Image image, vk::Format format);
  vk::UniqueDescriptorSet Allocate_Descriptor();
  vk::CommandBuffer Allocate_CommandBuffer(vk::CommandPool used);
  void Record_Manipulations(vk::CommandBuffer& rec, vk::Pipeline bind_pipeline, vk::DescriptorSet bind_dset, vk::Image img, vk::Extent2D img_size);
  void Record_FinalCopy(vk::CommandBuffer& rec, vk::Buffer dst, vk::Image src, vk::Extent2D img_size);

 public:
  image_manipulator() = default;
  image_manipulator(image_manipulator&& move) noexcept = default;
  image_manipulator& operator=(image_manipulator&& move) noexcept = default;
  explicit image_manipulator(
      pngine::device& handle,
      const vk::UniqueBuffer& staged_buffer,
      vk::Extent2D src_image_size);
  void clip_image(vk::Offset2D offset, vk::Extent2D size);
  void scale_image(float scaleX, float scaleY);
  void rotate_image(float rotate_angle_radians);
  vk::ImageView get_first_image_view() const;
  vk::Extent2D get_first_image_size() const;
  ~image_manipulator() noexcept = default;
};

image_manipulator::image_manipulator(
    pngine::device& handle,
    const vk::UniqueBuffer& staged_buffer,
    vk::Extent2D src_image_size)
    : m_device(&handle), m_first_image_size(src_image_size) {
  /* check max image size for gpu */
  const auto props = m_device->get_physdev().getImageFormatProperties(
      vk::Format::eR8Unorm,
      vk::ImageType::e2D,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled,
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
      vk::Format::eR8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  /* image view for input image */
  m_first_image_view = Create_ImageView(m_first_image.get(), vk::Format::eR8Unorm);

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
  auto transfer_buffer = Allocate_CommandBuffer(m_transfer_pool.get());

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
  vk::ImageMemoryBarrier transferDst_to_shaderRD{};
  transferDst_to_shaderRD.sType = vk::StructureType::eImageMemoryBarrier;
  transferDst_to_shaderRD.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferDst_to_shaderRD.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  transferDst_to_shaderRD.image = m_first_image.get();
  transferDst_to_shaderRD.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  transferDst_to_shaderRD.oldLayout = vk::ImageLayout::eTransferDstOptimal;
  transferDst_to_shaderRD.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
  transferDst_to_shaderRD.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  transferDst_to_shaderRD.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  using stg = vk::PipelineStageFlagBits;
  transfer_buffer.pipelineBarrier(
      stg::eTransfer,
      stg::eFragmentShader,
      vk::DependencyFlags{},
      0u,
      nullptr,
      0u,
      nullptr,
      1u,
      &transferDst_to_shaderRD);
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

void image_manipulator::rotate_image(float rotate_angle_radians) {
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

  auto [second_image, second_image_memory] = Create_Second_Image(second_image_size);
  auto second_image_view = Create_ImageView(second_image.get(), vk::Format::eR8Unorm);

  /* аллокация и запись дескрипторов */
  auto rotate_descr_set = Allocate_Descriptor();
  Update_Descriptor_Set(rotate_descr_set.get(), sizeof(pngine::rotating::params), second_image_view.get());

  /* запись команд */
  auto compute_buffer = Allocate_CommandBuffer(m_compute_pool.get());
  Record_Manipulations(compute_buffer, m_rotating_pipeline.get(), rotate_descr_set.get(), second_image.get(), second_image_size);

  vk::SubmitInfo submit_comp_info{};
  submit_comp_info.sType = vk::StructureType::eSubmitInfo;
  submit_comp_info.pCommandBuffers = &compute_buffer;
  submit_comp_info.commandBufferCount = 1u;

  const auto compute_queue = m_device->get_compute_queue();
  vk::Result r = compute_queue.submit(1u, &submit_comp_info, vk::Fence{});
  if (r != vk::Result::eSuccess)
    throw std::runtime_error("Image manipulator: не удалось сделать запись в вычислительную очередь успешно!");

  const auto start = std::chrono::high_resolution_clock::now();
  compute_queue.waitIdle();
  const auto end = std::chrono::high_resolution_clock::now();
  const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
  std::cout << "Rotating elapsed: " << time << '\n';

  m_device->get().freeCommandBuffers(m_compute_pool.get(), 1u, &compute_buffer);

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_size = second_image_size, m_first_image_memory = std::move(second_image_memory);
}

void image_manipulator::scale_image(float scaleX, float scaleY) {
  if (m_device == nullptr) // проверка на валидность класса
    throw std::runtime_error("image_manipulator:: объект не проининициализирован. Требуется валидный объект!");
  /* uniform buffer with params */
  void* const mapped = m_device->get().mapMemory(m_uniform_params_memory.get(), 0u, sizeof(pngine::scaling::params));
  const pngine::scaling::params params_host_buffer{glm::vec2(scaleX, scaleY)};
  ::memcpy(mapped, &params_host_buffer, sizeof(pngine::scaling::params));
  m_device->get().unmapMemory(m_uniform_params_memory.get());

  /* allocating output image storage memory */
  const auto second_image_size = pngine::force_clamp_image_extent(
      vk::Extent2D(std::roundf(m_first_image_size.width * scaleX), std::roundf(m_first_image_size.height * scaleY)),
      m_max_image_size);

  auto [second_image, second_image_memory] = Create_Second_Image(second_image_size);
  auto second_image_view = Create_ImageView(second_image.get(), vk::Format::eR8Unorm);

  /* аллокация и запись дескрипторов */
  auto scale_descr_set = Allocate_Descriptor();
  Update_Descriptor_Set(scale_descr_set.get(), sizeof(pngine::scaling::params), second_image_view.get());

  /* запись команд */
  auto compute_buffer = Allocate_CommandBuffer(m_compute_pool.get());
  Record_Manipulations(compute_buffer, m_scaling_pipeline.get(), scale_descr_set.get(), second_image.get(), second_image_size);

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

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_size = second_image_size, m_first_image_memory = std::move(second_image_memory);
}

void image_manipulator::clip_image(vk::Offset2D offset, vk::Extent2D size) {
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

  auto [second_image, second_image_memory] = Create_Second_Image(second_image_size);
  auto second_image_view = Create_ImageView(second_image.get(), vk::Format::eR8Unorm);

  /* аллокация и запись дескрипторов */
  auto clip_descr_set = Allocate_Descriptor();
  Update_Descriptor_Set(clip_descr_set.get(), sizeof(pngine::clipping::params), second_image_view.get());

  /* запись команд */
  auto compute_buffer = Allocate_CommandBuffer(m_compute_pool.get());
  Record_Manipulations(compute_buffer, m_clipping_pipeline.get(), clip_descr_set.get(), second_image.get(), second_image_size);

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

  m_first_image = std::move(second_image), m_first_image_view = std::move(second_image_view);
  m_first_image_size = second_image_size, m_first_image_memory = std::move(second_image_memory);
}
void image_manipulator::Update_Descriptor_Set(vk::DescriptorSet set, uint32_t uniform_size, vk::ImageView second_view) {
  vk::DescriptorBufferInfo d_params_info(m_uniform_params.get(), 0u, uniform_size);
  vk::DescriptorImageInfo d_in_image_info({}, m_first_image_view.get(), vk::ImageLayout::eGeneral);
  vk::DescriptorImageInfo d_out_image_info({}, second_view, vk::ImageLayout::eGeneral);

  vk::WriteDescriptorSet write_params{}, write_in_image{}, write_out_image{};
  write_params.sType = vk::StructureType::eWriteDescriptorSet;
  write_params.descriptorCount = 1u;
  write_params.pBufferInfo = &d_params_info;
  write_params.descriptorType = vk::DescriptorType::eUniformBuffer;
  write_params.dstSet = set;
  write_params.dstBinding = 0u;
  write_params.dstArrayElement = 0u;

  write_in_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_in_image.descriptorCount = 1u;
  write_in_image.pImageInfo = &d_in_image_info;
  write_in_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_in_image.dstSet = set;
  write_in_image.dstBinding = 1u;
  write_in_image.dstArrayElement = 0u;

  write_out_image.sType = vk::StructureType::eWriteDescriptorSet;
  write_out_image.descriptorCount = 1u;
  write_out_image.pImageInfo = &d_out_image_info;
  write_out_image.descriptorType = vk::DescriptorType::eStorageImage;
  write_out_image.dstSet = set;
  write_out_image.dstBinding = 2u;
  write_out_image.dstArrayElement = 0u;
  const auto writings = std::array{write_params, write_in_image, write_out_image};
  m_device->get().updateDescriptorSets(writings.size(), writings.data(), 0u, nullptr);
}

vk::UniqueImageView image_manipulator::Create_ImageView(vk::Image image, vk::Format format) {
  vk::ImageViewCreateInfo in_image_view_info{};
  in_image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  in_image_view_info.image = image;
  in_image_view_info.format = format;
  in_image_view_info.viewType = vk::ImageViewType::e2D;
  in_image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  return m_device->get().createImageViewUnique(in_image_view_info, nullptr);
}

vk::UniqueDescriptorSet image_manipulator::Allocate_Descriptor() {
  vk::DescriptorSetAllocateInfo alloc_sets_info{};
  alloc_sets_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
  alloc_sets_info.descriptorPool = m_descriptor_pool.get();
  alloc_sets_info.descriptorSetCount = 1u;
  alloc_sets_info.pSetLayouts = &m_descr_set_layout.get();

  auto descr_set = std::move(m_device->get().allocateDescriptorSetsUnique(alloc_sets_info).at(0u));
  return descr_set;
}

void image_manipulator::Record_Manipulations(vk::CommandBuffer& rec, vk::Pipeline bind_pipeline, vk::DescriptorSet bind_dset, vk::Image img, vk::Extent2D img_size) {
  vk::CommandBufferBeginInfo compute_begin_info{};
  compute_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  compute_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  rec.begin(compute_begin_info);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier shaderRD_to_general_1{};
  shaderRD_to_general_1.sType = vk::StructureType::eImageMemoryBarrier;
  shaderRD_to_general_1.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  shaderRD_to_general_1.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  shaderRD_to_general_1.image = m_first_image.get();
  shaderRD_to_general_1.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  shaderRD_to_general_1.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
  shaderRD_to_general_1.newLayout = vk::ImageLayout::eGeneral;
  shaderRD_to_general_1.srcAccessMask = vk::AccessFlagBits::eShaderRead;
  shaderRD_to_general_1.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  using stg = vk::PipelineStageFlagBits;
  rec.pipelineBarrier(
      stg::eFragmentShader, stg::eComputeShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &shaderRD_to_general_1);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier undef_to_general_2{};
  undef_to_general_2.sType = vk::StructureType::eImageMemoryBarrier;
  undef_to_general_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  undef_to_general_2.image = img;
  undef_to_general_2.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  undef_to_general_2.oldLayout = vk::ImageLayout::eUndefined;
  undef_to_general_2.newLayout = vk::ImageLayout::eGeneral;
  undef_to_general_2.srcAccessMask = vk::AccessFlags{};
  undef_to_general_2.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
  using stg = vk::PipelineStageFlagBits;
  rec.pipelineBarrier(
      stg::eTopOfPipe, stg::eComputeShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &undef_to_general_2);
  /* bindPipeline */
  rec.bindPipeline(vk::PipelineBindPoint::eCompute, bind_pipeline);
  /* bindDescriptorSets */
  rec.bindDescriptorSets(
      vk::PipelineBindPoint::eCompute, m_compute_pipeline_layout.get(), 0u, 1u, &bind_dset, 0u, nullptr);
  /* dispatch */
  rec.dispatch(std::ceilf(img_size.width / 16.f), std::ceilf(img_size.height / 16.f), 1u);
  /* pipelineBarrier */
  vk::ImageMemoryBarrier general_to_shaderRD_2{};
  general_to_shaderRD_2.sType = vk::StructureType::eImageMemoryBarrier;
  general_to_shaderRD_2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_shaderRD_2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
  general_to_shaderRD_2.image = img;
  general_to_shaderRD_2.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);
  general_to_shaderRD_2.oldLayout = vk::ImageLayout::eGeneral;
  general_to_shaderRD_2.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
  general_to_shaderRD_2.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
  general_to_shaderRD_2.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  rec.pipelineBarrier(
      stg::eComputeShader, stg::eFragmentShader, vk::DependencyFlags{}, 0u, nullptr, 0u, nullptr, 1u, &general_to_shaderRD_2);

  rec.end();
}
vk::CommandBuffer image_manipulator::Allocate_CommandBuffer(vk::CommandPool used) {
  vk::CommandBufferAllocateInfo comp_alloc_info{};
  comp_alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  comp_alloc_info.commandPool = used;
  comp_alloc_info.level = vk::CommandBufferLevel::ePrimary;
  comp_alloc_info.commandBufferCount = 1u;
  auto buffers = m_device->get().allocateCommandBuffers(comp_alloc_info);
  auto buffer = std::move(buffers[0u]);
  return buffer;
}

pngine::img_and_mem image_manipulator::Create_Second_Image(vk::Extent2D size) {
  auto pair = m_device->create_image(
      size,
      vk::Format::eR8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled,
      vk::MemoryPropertyFlagBits::eDeviceLocal);
  return pair;
}

vk::ImageView image_manipulator::get_first_image_view() const {
  return m_first_image_view.get();
}
vk::Extent2D image_manipulator::get_first_image_size() const {
  return m_first_image_size;
}

} // namespace pngine
} // namespace csc
