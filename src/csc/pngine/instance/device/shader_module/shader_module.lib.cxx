module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
export module csc.pngine.instance.device.shader_module;

import vulkan_hpp;

import csc.pngine.commons.utility.shader_module;

import stl.vector;
import stl.string_view;
import stl.stdexcept;

export namespace csc {
namespace pngine {
class shader_module {
 private:
  const vk::Device* m_keep_device = nullptr;

  std::vector<uint8_t> m_byte_code_spv{};
  vk::ShaderModule m_shader_module{};
  vk::Bool32 m_is_created = false;

 public:
  explicit shader_module() = default;
  explicit shader_module(const vk::Device& device, std::string_view compiled_filename);
  ~shader_module() noexcept;
  shader_module(shader_module&& move) noexcept;
  shader_module& operator=(shader_module&& move) noexcept;
  void clear() noexcept;
  vk::ShaderModule get() const;
  vk::PipelineShaderStageCreateInfo create_shader_stage(
      std::string_view entry_point,
      vk::ShaderStageFlagBits selected_stage) const;
};

shader_module& shader_module::operator=(shader_module&& move) noexcept {
  if (&move == this)
    return *this;
  clear();
  m_shader_module = move.m_shader_module;
  m_keep_device = move.m_keep_device;
  m_byte_code_spv = std::move(move.m_byte_code_spv);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

shader_module::shader_module(shader_module&& move) noexcept
    : m_keep_device(move.m_keep_device),
      m_byte_code_spv(std::move(move.m_byte_code_spv)),
      m_shader_module(move.m_shader_module),
      m_is_created(std::exchange(move.m_is_created, false)) {
}

shader_module::shader_module(const vk::Device& device, std::string_view compiled_filename) : m_keep_device(&device) {
  m_byte_code_spv = pngine::read_spirv_shader_from_file(compiled_filename);

  vk::ShaderModuleCreateInfo description{};
  description.sType = vk::StructureType::eShaderModuleCreateInfo;
  description.codeSize = m_byte_code_spv.size();
  description.pCode = reinterpret_cast<const uint32_t*>(m_byte_code_spv.data());

  m_shader_module = m_keep_device->createShaderModule(description, nullptr);
  m_is_created = true;
}

void shader_module::clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyShaderModule(m_shader_module, nullptr);
    m_is_created = false;
  }
}

vk::ShaderModule shader_module::get() const {
  return m_shader_module;
}
vk::PipelineShaderStageCreateInfo shader_module::create_shader_stage(
    std::string_view entry_point,
    vk::ShaderStageFlagBits selected_stage) const {
  if (m_is_created == false)
    throw std::runtime_error("ShaderModule: невозможно создать PipelineShaderStageInfo, пока не создан ShaderModule");
  vk::PipelineShaderStageCreateInfo description{};
  description.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  description.pNext = nullptr;
  description.stage = selected_stage;
  description.module = m_shader_module;
  description.pName = entry_point.data();
  return description;
}

shader_module::~shader_module() noexcept {
  clear();
}

} // namespace pngine
} // namespace csc
