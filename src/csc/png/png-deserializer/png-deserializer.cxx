module;
#include <cstdint>

#include <algorithm>
#include <fstream>
#include <variant>
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

csc::v_section init_section(const csc::chunk& ch) {
  std::string chunk_name = {ch.chunk_name.cbegin(), ch.chunk_name.cend()};
  if (chunk_name == "IHDR")
    return csc::v_section(IHDR());
  else if (chunk_name == "PLTE")
    return csc::v_section(PLTE());
  else if (chunk_name == "IDAT")
    return csc::v_section(IDAT());
  else if (chunk_name == "IEND")
    return csc::v_section(IEND());
  else
    return csc::v_section(IEND());
}

csc::chunk read_chunk_from_ifstream(std::ifstream& is) {
  csc::chunk bufferized;
  // длина недесериализованного блока в чанке
  is.read(reinterpret_cast<char*>(&bufferized.contained_length), sizeof(bufferized.contained_length));
  bufferized.contained_length = csc::swap_endian(bufferized.contained_length);
  // имя чанка
  is.read(reinterpret_cast<char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись недесериализованного блока в чанк
  [[likely]] if (bufferized.contained_length != 0u) {
    bufferized.data.reserve(bufferized.contained_length);
    is.read(reinterpret_cast<char*>(bufferized.data.data()), bufferized.contained_length);
  }
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(bufferized.crc_adler));
  bufferized.crc_adler = csc::swap_endian(bufferized.crc_adler);
  return bufferized;
}
void check_for_chunk_errors(const csc::png_t& image) {
  const csc::IHDR& header = std::get<csc::IHDR>(image.m_structured[0]);
  const csc::v_sections& sns = image.m_structured;
  const auto is_plte_type = [](const v_section& sn) { return std::holds_alternative<csc::PLTE>(sn); };
  const auto is_idat_type = [](const v_section& sn) { return std::holds_alternative<csc::IDAT>(sn); };

  const auto plte_pos = std::find_if(sns.cbegin(), sns.cend(), is_plte_type);
  [[unlikely]] if (!std::holds_alternative<csc::IHDR>(sns.front()))
    throw std::domain_error("Блок IHDR не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (!std::holds_alternative<csc::IEND>(sns.back()))
    throw std::domain_error("Блок IEND не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos == sns.cend() && header.color_type() == color_type_t::indexed)
    throw std::domain_error("Для индексного изображения требуется палитра!");
  const auto idat_pos = std::find_if(sns.cbegin(), sns.cend(), is_idat_type);
  [[unlikely]] if (idat_pos == sns.cend())
    throw std::domain_error("Блок IDAT не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos != sns.end() && idat_pos - plte_pos <= 0)
    throw std::domain_error("Блок IDAT должен располагаться после блока PLTE при его наличии!");
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
    uint32_t result = std::visit(csc::f_construct(chunk, image.m_structured), section);
    if (result != 0u)
      throw std::domain_error("Ошибка в представлении сектора: " + std::string(chunk.chunk_name.data()));
    image.m_structured.emplace_back(std::move(section));
  }
  // проверка правильности расположения секторов
  try {
    csc::check_for_chunk_errors(image);
  } catch (const std::domain_error& e) {
    throw std::domain_error(std::string("PNG-изображение не прошло проверку: ") + e.what());
  }
  png_fs.close();
  return image;
}
} // namespace csc
