module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.pipeline_layout:impl;

export import vulkan_hpp;

import stl.vector;

import :utility;
namespace csc {
namespace pngine {
class pipeline_layout_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::PipelineLayout m_pipeline_layout{};
  vk::Bool32 m_is_created = false;

 public:
  explicit pipeline_layout_impl() = default;
  explicit pipeline_layout_impl(const vk::Device& device);
  ~pipeline_layout_impl() noexcept;
  pipeline_layout_impl(pipeline_layout_impl&& move) noexcept;
  pipeline_layout_impl& operator=(pipeline_layout_impl&& move) noexcept;
  vk::PipelineLayout do_get() const;
  void do_clear() noexcept;
};

pipeline_layout_impl::pipeline_layout_impl(pipeline_layout_impl&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_pipeline_layout(move.m_pipeline_layout),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
pipeline_layout_impl& pipeline_layout_impl::operator=(pipeline_layout_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_pipeline_layout = move.m_pipeline_layout;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

pipeline_layout_impl::pipeline_layout_impl(const vk::Device& device) : m_keep_device(&device) {
  vk::PipelineLayoutCreateInfo description{};
  description.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  description.setLayoutCount = 0u;
  description.pushConstantRangeCount = 0u;

  m_pipeline_layout = m_keep_device->createPipelineLayout(description, nullptr);
  m_is_created = true;
}

void pipeline_layout_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyPipelineLayout(m_pipeline_layout, nullptr);
    m_is_created = false;
  }
}

vk::PipelineLayout pipeline_layout_impl::do_get() const {
  return m_pipeline_layout;
}
pipeline_layout_impl::~pipeline_layout_impl() noexcept {
  do_clear();
}

} // namespace pngine
} // namespace csc
