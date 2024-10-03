module;
#include <bits/move.h>
#include <utility>
#include <cstdint>
module csc.pngine.instance.device.shader_module:impl;

export import vulkan_hpp;

import csc.pngine.commons.utility.shader_module;

import stl.vector;
import stl.string_view;

import :utility;
namespace csc {
namespace pngine {
class shader_module_impl {
 private:
  const vk::Device* m_keep_device = nullptr;

  std::vector<uint8_t> m_byte_code_spv{};
  vk::ShaderModule m_shader_module{};
  vk::Bool32 m_is_created = false;

 public:
  explicit shader_module_impl() = default;
  explicit shader_module_impl(const vk::Device& device, std::string_view compiled_filename);
  ~shader_module_impl() noexcept = default;
  shader_module_impl(shader_module_impl&& move) noexcept;
  shader_module_impl& operator=(shader_module_impl&& move) noexcept;
  void do_clear() noexcept;
};

shader_module_impl& shader_module_impl::operator=(shader_module_impl&& move) noexcept {
  if (&move == this)
    return *this;
  do_clear();
  m_shader_module = move.m_shader_module;
  m_keep_device = move.m_keep_device;
  m_byte_code_spv = std::move(move.m_byte_code_spv);
  m_is_created = std::exchange(move.m_is_created, false);
  return *this;
}

shader_module_impl::shader_module_impl(shader_module_impl&& move) noexcept : 
  m_keep_device(move.m_keep_device),
  m_byte_code_spv(std::move(move.m_byte_code_spv)),
  m_shader_module(move.m_shader_module),
  m_is_created(std::exchange(move.m_is_created, false)) {}

shader_module_impl::shader_module_impl(const vk::Device& device, std::string_view compiled_filename)
    : m_keep_device(&device) {
  m_byte_code_spv = pngine::read_spirv_shader_from_file(compiled_filename);

  vk::ShaderModuleCreateInfo description{};
  description.sType = vk::StructureType::eShaderModuleCreateInfo;
  description.codeSize = m_byte_code_spv.size();
  description.pCode = reinterpret_cast<const uint32_t*>(m_byte_code_spv.data());

  m_shader_module = m_keep_device->createShaderModule(description, nullptr);
  m_is_created = true;
}

void shader_module_impl::do_clear() noexcept {
  if (m_is_created != false) {
    m_keep_device->destroyShaderModule(m_shader_module, nullptr);
    m_is_created = false;
  }
}

} // namespace pngine
} // namespace csc
