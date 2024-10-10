module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.graphics_pipeline:impl;

export import vulkan_hpp;

import csc.pngine.commons.utility.graphics_pipeline;
import stl.vector;

import :utility;
namespace csc {
namespace pngine {
class graphics_pipeline_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  pngine::graphics_pipeline_config m_config{};
  vk::Pipeline m_graphics_pipeline{};
  vk::Bool32 m_is_created;

 public:
  template <pngine::c_graphics_pipeline_config Config>
  explicit graphics_pipeline_impl(const vk::Device& device, const vk::PipelineLayout& layout, Config&& config);
  ~graphics_pipeline_impl() noexcept;
  graphics_pipeline_impl(graphics_pipeline_impl&& move) noexcept;
  graphics_pipeline_impl& operator=(graphics_pipeline_impl&& move) noexcept;
  void do_clear() noexcept;
};

graphics_pipeline_impl::graphics_pipeline_impl(graphics_pipeline_impl&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_config(std::move(move.m_config)),
      m_graphics_pipeline(move.m_graphics_pipeline),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
graphics_pipeline_impl& graphics_pipeline_impl::operator=(graphics_pipeline_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_graphics_pipeline = move.m_graphics_pipeline;
  m_keep_device = move.m_keep_device;
  m_config = std::move(move.m_config);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

template <pngine::c_graphics_pipeline_config Config>
graphics_pipeline_impl::graphics_pipeline_impl(
    const vk::Device& device,
    const vk::PipelineLayout& layout,
    Config&& config)
    : m_keep_device(&device), m_config(std::forward<Config>(config)), m_is_created(false) {
  vk::GraphicsPipelineCreateInfo description;
  m_graphics_pipeline = m_keep_device->createGraphicsPipeline(vk::PipelineCache(), description, nullptr).value;
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
