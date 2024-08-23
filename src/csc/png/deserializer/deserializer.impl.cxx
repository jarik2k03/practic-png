module;
#include <cstdint>

#include <algorithm>
export module csc.png.deserializer:impl;
import csc.stl_wrap.string_view;
import csc.stl_wrap.string;
import csc.stl_wrap.vector;
import csc.stl_wrap.fstream;
import csc.stl_wrap.stdexcept;
import csc.stl_wrap.variant;
import csc.stl_wrap.ios;
import csc.stl_wrap.iostream;

export import csc.png.png_t;
import csc.png.deserializer.consume_chunk.inflater;
import csc.png.commons.utils;
import csc.png.commons.chunk;
import csc.png.deserializer.consume_chunk;

namespace csc {

class deserializer_impl {
 public:
  csc::png_t do_deserialize(csc::string_view filepath);
};

csc::v_section init_section(const csc::chunk& ch) {
  csc::string chunk_name = {ch.chunk_name.cbegin(), ch.chunk_name.cend()};
  if (chunk_name == "IHDR")
    return csc::v_section(csc::IHDR());
  else if (chunk_name == "PLTE")
    return csc::v_section(csc::PLTE());
  else if (chunk_name == "IDAT")
    return csc::v_section(csc::IDAT());
  else if (chunk_name == "IEND")
    return csc::v_section(csc::IEND());
  else
    return csc::v_section(csc::IEND());
}

csc::chunk read_chunk_from_ifstream(csc::ifstream& is) {
  csc::chunk bufferized;
  // длина недесериализованного блока в чанке
  uint32_t chunk_size;
  is.read(reinterpret_cast<char*>(&chunk_size), sizeof(chunk_size));
  chunk_size = csc::swap_endian(chunk_size);
  // имя чанка
  is.read(reinterpret_cast<char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись недесериализованного блока в чанк
  bufferized.data.resize(chunk_size); // для подготовки пространства
  is.read(reinterpret_cast<char*>(bufferized.data.data()), chunk_size);
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(bufferized.crc_adler));
  bufferized.crc_adler = csc::swap_endian(bufferized.crc_adler);
  return bufferized;
}
void check_for_chunk_errors(const csc::png_t& image) {
  const csc::IHDR& header = csc::get<csc::IHDR>(image.m_structured[0]);
  const csc::v_sections& sns = image.m_structured;
  const auto is_plte_type = [](const v_section& sn) { return csc::holds_alternative<csc::PLTE>(sn); };
  const auto is_idat_type = [](const v_section& sn) { return csc::holds_alternative<csc::IDAT>(sn); };

  const auto plte_pos = std::find_if(sns.cbegin(), sns.cend(), is_plte_type);
  [[unlikely]] if (!csc::holds_alternative<csc::IHDR>(sns.front()))
    throw csc::domain_error("Блок IHDR не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (!csc::holds_alternative<csc::IEND>(sns.back()))
    throw csc::domain_error("Блок IEND не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos == sns.cend() && header.color_type == color_type_t::indexed)
    throw csc::domain_error("Для индексного изображения требуется палитра!");
  const auto idat_pos = std::find_if(sns.cbegin(), sns.cend(), is_idat_type);
  [[unlikely]] if (idat_pos == sns.cend())
    throw csc::domain_error("Блок IDAT не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos != sns.end() && idat_pos - plte_pos <= 0)
    throw csc::domain_error("Блок IDAT должен располагаться после блока PLTE при его наличии!");
}

csc::png_t deserializer_impl::do_deserialize(csc::string_view filepath) {
  csc::ifstream png_fs;
  png_fs.open(filepath.data(), csc::ios_base::binary);
  csc::png_t image;
  if (!png_fs.is_open())
    throw csc::runtime_error("Не существует файла в указанной директории!");

  png_fs.read(reinterpret_cast<char*>(&image.start()), sizeof(csc::png_signature));
  if (image.start() != csc::png_signature())
    throw csc::runtime_error("Ошибка чтения png - предзаголовочная подпись не проинициализирована!");

  csc::inflater z_stream;
  while (png_fs.peek() != -1) {
    const auto chunk = csc::read_chunk_from_ifstream(png_fs);
    auto section = csc::init_section(chunk);
    const auto result =
        csc::visit(csc::f_consume_chunk(chunk, image.m_structured, z_stream, image.m_image_data), section);
    if (result != section_code_t::success) {
      const csc::string err_msg = "Ошибка в представлении сектора: " + csc::string(chunk.chunk_name.data(), 4);
      throw csc::domain_error(err_msg.c_str());
    }
    image.m_structured.emplace_back(std::move(section));
  }
  // проверка правильности расположения секторов
  try {
    csc::check_for_chunk_errors(image);
  } catch (const csc::domain_error& e) {
    const csc::string err_msg = "PNG-изображение не прошло проверку: " + csc::string(e.what());
    throw csc::domain_error(err_msg.c_str());
  }
  png_fs.close();
  return image;
}
} // namespace csc
