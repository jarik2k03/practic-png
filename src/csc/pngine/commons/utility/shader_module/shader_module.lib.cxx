module;
#include <cstdint>
export module csc.pngine.commons.utility.shader_module;

import stl.vector;
import stl.string_view;
import stl.string;
import stl.fstream;
import stl.ios;
import stl.stdexcept;

import vulkan_hpp;

export namespace csc {
namespace pngine {

std::vector<uint8_t> read_spirv_shader_from_file(std::string_view filename) {
  std::ifstream shader_stream;
  shader_stream.open(filename.data(), std::ios::binary | std::ios::ate);
  [[unlikely]] if (!shader_stream.is_open())
    throw std::runtime_error("Не найден шейдер-модуль: " + std::string(filename.data()));
  const uint64_t file_size = shader_stream.tellg();
  std::vector<uint8_t> shader(file_size);
  shader_stream.seekg(0u);
  shader_stream.read(reinterpret_cast<char*>(shader.data()), file_size);
  shader_stream.close();
  return shader;
}

} // namespace pngine
} // namespace csc
