module;
#include <bits/stl_construct.h>
#include <climits>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <vector>
export module csc.png:deserializer;
export import csc.png.png_t;

namespace csc {}

export namespace csc {

class deserializer {
 public:
  csc::png_t deserialize(std::string_view filepath);
};
} // namespace csc

namespace csc {
csc::chunk read_chunk_from_ifstream (std::ifstream& is) {
  csc::chunk bufferized;
  // длина недесериализованного блока в чанке
  is.read(reinterpret_cast<char*>(&bufferized.contained_length), sizeof(uint32_t));
  bufferized.contained_length = csc::swap_endian(bufferized.contained_length);
  // имя чанка
  is.read(reinterpret_cast<char*>(&bufferized.chunk_name), sizeof(uint32_t));
  // запись недесериализованного блока в чанк
  [[likely]] if (bufferized.contained_length != 0u) {
    bufferized.data.reserve(bufferized.contained_length);
    is.read(reinterpret_cast<char*>(bufferized.data.data()), bufferized.contained_length);
  }
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(uint32_t));
  bufferized.crc_adler = csc::swap_endian(bufferized.crc_adler);
  return bufferized;
} 
    
csc::png_t deserializer::deserialize(std::string_view filepath) {
  std::ifstream png_fs;
  png_fs.open(filepath.data(), std::ios_base::binary);
  csc::png_t image;
  if (!png_fs.is_open())
    throw std::runtime_error("Не существует файла в указанной директории!");
  png_fs.read(reinterpret_cast<char*>(&image.start()), sizeof(csc::SUBSCRIBE));
  if (image.start() != csc::SUBSCRIBE())
    throw std::runtime_error("Ошибка чтения png - предзаголовочная подпись не проинициализирована!");
 
  while (png_fs) {
    const auto chunk = csc::read_chunk_from_ifstream(png_fs);
    auto section = csc::init_section(chunk);
    image.m_structured.emplace_back(std::move(section));
  }
  png_fs.close();
  return image;
}
} // namespace csc
