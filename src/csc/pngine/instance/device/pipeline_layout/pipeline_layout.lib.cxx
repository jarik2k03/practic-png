module;
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.pipeline_layout;

import vulkan_hpp;

import stl.vector;

export namespace csc {
namespace pngine {
class pipeline_layout {
 private:
  const vk::Device* m_keep_device = nullptr;

  vk::PipelineLayout m_pipeline_layout{};
  vk::Bool32 m_is_created = false;

 public:
  explicit pipeline_layout() = default;
  explicit pipeline_layout(const vk::Device& device);
  ~pipeline_layout() noexcept;
  pipeline_layout(pipeline_layout&& move) noexcept;
  pipeline_layout& operator=(pipeline_layout&& move) noexcept;
  vk::PipelineLayout get() const;
  void clear() noexcept;
};

pipeline_layout::pipeline_layout(pipeline_layout&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_pipeline_layout(move.m_pipeline_layout),
      m_is_created(std::exchange(move.m_is_created, false)) {
}
pipeline_layout& pipeline_layout::operator=(pipeline_layout&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_pipeline_layout = move.m_pipeline_layout;
  m_keep_device = move.m_keep_device;
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

pipeline_layout::pipeline_layout(const vk::Device& device) : m_keep_device(&device) {
  vk::PipelineLayoutCreateInfo description{};
  description.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  description.setLayoutCount = 0u;
  description.pushConstantRangeCount = 0u;

  m_pipeline_layout = m_keep_device->createPipelineLayout(description, nullptr);
  m_is_created = true;
}

void pipeline_layout::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyPipelineLayout(m_pipeline_layout, nullptr);
    m_is_created = false;
  }
}

vk::PipelineLayout pipeline_layout::get() const {
  return m_pipeline_layout;
}
pipeline_layout::~pipeline_layout() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
