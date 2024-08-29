module;
#include <cstdint>

#include <algorithm>
export module csc.png.deserializer:impl;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.string;
import cstd.stl_wrap.vector;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.ios;
import cstd.stl_wrap.iostream;

export import csc.png.picture;
import csc.png.deserializer.consume_chunk.inflater;
import csc.png.commons.chunk;
import csc.png.deserializer.consume_chunk;
import csc.png.deserializer.consume_chunk.buf_reader;

namespace csc {
using cstd::operator==;
using cstd::operator+;
using cstd::operator<<;

class deserializer_impl {
 public:
  csc::picture do_deserialize(cstd::string_view filepath);
};

csc::v_section init_section(const csc::chunk& ch) {
  const cstd::string chunk_name = {ch.chunk_name.cbegin(), ch.chunk_name.cend()};
  if (chunk_name == "IHDR")
    return csc::v_section(csc::IHDR());
  else if (chunk_name == "PLTE")
    return csc::v_section(csc::PLTE());
  else if (chunk_name == "IDAT")
    return csc::v_section(csc::IDAT());
  else if (chunk_name == "IEND")
    return csc::v_section(csc::IEND());
  else if (chunk_name == "bKGD")
    return csc::v_section(csc::bKGD());
  else if (chunk_name == "tIME")
    return csc::v_section(csc::tIME());
  else if (chunk_name == "cHRM")
    return csc::v_section(csc::cHRM());
  else if (chunk_name == "gAMA")
    return csc::v_section(csc::gAMA());
  else if (chunk_name == "hIST")
    return csc::v_section(csc::hIST());
  else if (chunk_name == "pHYs")
    return csc::v_section(csc::pHYs());
  else
    return csc::v_section(csc::IEND());
}

csc::chunk read_chunk_from_ifstream(cstd::ifstream& is) {
  csc::chunk bufferized;
  // длина недесериализованного блока в чанке
  uint32_t chunk_size;
  is.read(reinterpret_cast<char*>(&chunk_size), sizeof(chunk_size));
  chunk_size = csc::from_be_to_system_endian(chunk_size);
  // имя чанка
  is.read(reinterpret_cast<char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись недесериализованного блока в чанк
  bufferized.data.resize(chunk_size); // для подготовки пространства
  is.read(reinterpret_cast<char*>(bufferized.data.data()), chunk_size);
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(bufferized.crc_adler));
  bufferized.crc_adler = csc::from_be_to_system_endian(bufferized.crc_adler);
  return bufferized;
}
void check_for_chunk_errors(const csc::picture& image) {
  const csc::IHDR& header = cstd::get<csc::IHDR>(image.m_structured[0]);
  const csc::v_sections& sns = image.m_structured;
  const auto is_plte_type = [](const v_section& sn) { return cstd::holds_alternative<csc::PLTE>(sn); };
  const auto is_idat_type = [](const v_section& sn) { return cstd::holds_alternative<csc::IDAT>(sn); };

  const auto plte_pos = std::find_if(sns.cbegin(), sns.cend(), is_plte_type);
  [[unlikely]] if (!cstd::holds_alternative<csc::IHDR>(sns.front()))
    throw cstd::domain_error("Блок IHDR не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (!cstd::holds_alternative<csc::IEND>(sns.back()))
    throw cstd::domain_error("Блок IEND не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos == sns.cend() && header.color_type == e_color_type::indexed)
    throw cstd::domain_error("Для индексного изображения требуется палитра!");
  const auto idat_pos = std::find_if(sns.cbegin(), sns.cend(), is_idat_type);
  [[unlikely]] if (idat_pos == sns.cend())
    throw cstd::domain_error("Блок IDAT не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos != sns.end() && idat_pos - plte_pos <= 0)
    throw cstd::domain_error("Блок IDAT должен располагаться после блока PLTE при его наличии!");
}

csc::picture deserializer_impl::do_deserialize(cstd::string_view filepath) {
  cstd::ifstream png_fs;
  png_fs.open(filepath.data(), cstd::ios_base::binary);
  csc::picture image;
  if (!png_fs.is_open())
    throw cstd::runtime_error("Не существует файла в указанной директории!");

  png_fs.read(reinterpret_cast<char*>(&image.start()), sizeof(csc::png_signature));
  if (image.start() != csc::png_signature())
    throw cstd::runtime_error("Ошибка чтения png - предзаголовочная подпись не проинициализирована!");

  csc::inflater z_stream;
  while (png_fs.peek() != -1) {
    const auto chunk = csc::read_chunk_from_ifstream(png_fs);
    auto section = csc::init_section(chunk);
    auto consume_chunk = csc::f_consume_chunk(chunk, image.m_structured, z_stream, image.m_image_data);
    const auto result = cstd::visit(consume_chunk, section);
    if (result != e_section_code::success) {
      const cstd::string err_msg = "Ошибка в представлении сектора: " + cstd::string(chunk.chunk_name.data(), 4);
      throw cstd::domain_error(err_msg);
    }
    image.m_structured.emplace_back(std::move(section));
  }
  // проверка правильности расположения секторов
  try {
    csc::check_for_chunk_errors(image);
  } catch (const cstd::domain_error& e) {
    using namespace cstd::string_literals;
    throw cstd::domain_error("PNG-изображение не прошло проверку: "_s + e.what());
  }
  png_fs.close();
  return image;
}
} // namespace csc
