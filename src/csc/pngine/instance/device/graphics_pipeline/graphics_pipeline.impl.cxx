module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.graphics_pipeline:impl;

export import vulkan_hpp;

import stl.vector;

import :utility;
namespace csc {
namespace pngine {
class graphics_pipeline_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::Pipeline m_graphics_pipeline{};
  vk::Bool32 m_is_created;

 public:
  explicit graphics_pipeline_impl(const vk::Device& device);
  ~graphics_pipeline_impl() noexcept;
  graphics_pipeline_impl(graphics_pipeline_impl&& move) noexcept;
  graphics_pipeline_impl& operator=(graphics_pipeline_impl&& move) noexcept;
  void do_create();
  void do_clear() noexcept;
};

graphics_pipeline_impl::graphics_pipeline_impl(graphics_pipeline_impl&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_graphics_pipeline(move.m_graphics_pipeline),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
graphics_pipeline_impl& graphics_pipeline_impl::operator=(graphics_pipeline_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_graphics_pipeline = move.m_graphics_pipeline;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

graphics_pipeline_impl::graphics_pipeline_impl(const vk::Device& device) : m_keep_device(&device), m_is_created(false) {
}

void graphics_pipeline_impl::do_create() {
  if (m_is_created == false) {
    vk::GraphicsPipelineCreateInfo description;
    m_graphics_pipeline = m_keep_device->createGraphicsPipeline(vk::PipelineCache(), description, nullptr).value;
    m_is_created = true;
  }
}

void graphics_pipeline_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyPipeline(m_graphics_pipeline, nullptr);
    m_is_created = false;
  }
}

graphics_pipeline_impl::~graphics_pipeline_impl() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
