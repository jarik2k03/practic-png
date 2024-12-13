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
  // layout
  vk::UniqueDescriptorSetLayout m_descr_set_layout;
  vk::UniquePipelineLayout m_compute_pipeline_layout;

  vk::UniqueCommandPool m_compute_pool;
  vk::UniqueCommandPool m_transfer_pool;

  // uniform params;
  vk::UniqueDeviceMemory m_uniform_params_memory;
  vk::UniqueBuffer m_uniform_params;
  // uniform readonly image2D in_image;
  vk::UniqueDeviceMemory m_in_image_memory;
  vk::UniqueImage m_in_image;
  vk::UniqueImageView m_in_image_view;
  // uniform writeonly image2D out_image;
  vk::UniqueDeviceMemory m_out_image_memory;
  vk::UniqueImage m_out_image;
  vk::UniqueImageView m_out_image_view;

 public:
  image_manipulator() = default;
  image_manipulator(image_manipulator&& move) noexcept = default;
  image_manipulator& operator=(image_manipulator&& move) noexcept = default;
  explicit image_manipulator(
      pngine::device& handle,
      const std::vector<uint8_t>& src_image,
      vk::Extent2D src_image_size);
  void clip_image(vk::Extent2D offset, vk::Extent2D size);
  ~image_manipulator() noexcept = default;
};

image_manipulator::image_manipulator(
    pngine::device& handle,
    const std::vector<uint8_t>& src_image,
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
  compute_pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
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
  des_pool_toolbox_info.maxSets = 32u; // гвоздь 10 см
  des_pool_toolbox_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

  m_descriptor_pool = m_device->get().createDescriptorPoolUnique(des_pool_toolbox_info, nullptr);

  /* allocating an empty uniform buffer */
  std::tie(m_uniform_params, m_uniform_params_memory) = m_device->create_buffer(
      128u, // зафиксирован максимальный размер параметров
      vk::BufferUsageFlagBits::eUniformBuffer,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  /* allocating input image storage memory */
  std::tie(m_in_image, m_in_image_memory) = m_device->create_image(
      src_image_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst,
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  /* creating compute and transfer command pools */
  m_compute_pool = m_device->create_compute_command_pool(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
  m_transfer_pool = m_device->create_transfer_command_pool(vk::CommandPoolCreateFlagBits::eTransient);
}

void image_manipulator::clip_image(vk::Extent2D offset, vk::Extent2D size) {
  if (m_device == nullptr) // проверка на валидность класса
    throw std::runtime_error("image_manipulator:: объект не проининициализирован. Требуется валидный объект!");
  /* uniform buffer with params */
  void* const mapped = m_device->get().mapMemory(m_uniform_params_memory.get(), 0u, sizeof(pngine::clipping::params));
  const pngine::clipping::params params_host_buffer{
      glm::uvec2(offset.width, offset.height), glm::uvec2(size.width, size.height)};
  ::memcpy(mapped, &params_host_buffer, sizeof(pngine::clipping::params));
  m_device->get().unmapMemory(m_uniform_params_memory.get());

  /* allocating input image storage memory */
  const auto clip_size = vk::Extent2D(size.width - offset.width, size.height - offset.height);
  std::tie(m_out_image, m_out_image_memory) = m_device->create_image(
      clip_size,
      vk::Format::eR8G8B8A8Unorm, // мы работаем с сырыми данными изображений, без гамма-наложений
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eStorage, // запись данных будет в шейдере
      vk::MemoryPropertyFlagBits::eDeviceLocal);

  /* image view for input image */
  vk::ImageViewCreateInfo in_image_view_info{};
  in_image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  in_image_view_info.image = m_in_image.get();
  in_image_view_info.format = vk::Format::eR8G8B8A8Unorm;
  in_image_view_info.viewType = vk::ImageViewType::e2D;
  in_image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  m_in_image_view = m_device->get().createImageViewUnique(in_image_view_info, nullptr);

  /* image view for output image */
  vk::ImageViewCreateInfo out_image_view_info{};
  out_image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  out_image_view_info.image = m_out_image.get();
  out_image_view_info.format = vk::Format::eR8G8B8A8Unorm;
  out_image_view_info.viewType = vk::ImageViewType::e2D;
  out_image_view_info.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u);

  m_out_image_view = m_device->get().createImageViewUnique(out_image_view_info, nullptr);

  /* создание и запись дескрипторов */
  vk::DescriptorSetAllocateInfo alloc_sets_info{};
  alloc_sets_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
  alloc_sets_info.descriptorPool = m_descriptor_pool.get();
  alloc_sets_info.descriptorSetCount = 1u;
  alloc_sets_info.pSetLayouts = &m_descr_set_layout.get();

  auto clip_descr_set = std::move(m_device->get().allocateDescriptorSetsUnique(alloc_sets_info).at(0u));

  vk::DescriptorBufferInfo d_params_info(m_uniform_params.get(), 0u, sizeof(pngine::clipping::params));
  vk::DescriptorImageInfo d_in_image_info({}, m_in_image_view.get(), vk::ImageLayout::eGeneral);
  vk::DescriptorImageInfo d_out_image_info({}, m_out_image_view.get(), vk::ImageLayout::eGeneral);

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
}

} // namespace pngine
} // namespace csc
