module;
#include <bits/stl_algo.h>
#include <bits/ranges_util.h>
#include <cstdint>
module csc.png.deserializer:utility;

import csc.png.deserializer.consume_chunk.buf_reader;
import csc.png.commons.chunk;
import cstd.stl_wrap.string;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import csc.png.picture;

namespace csc {

csc::v_section init_section(const csc::chunk& ch) {
  using cstd::operator==;
  const cstd::string chunk_name = {ch.chunk_name.cbegin(), ch.chunk_name.cend()};
  if (chunk_name == "IHDR")
    return csc::v_section(csc::IHDR());
  else if (chunk_name == "PLTE")
    return csc::v_section(csc::PLTE());
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
    return csc::v_section(csc::dummy());
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
  bufferized.buffer = csc::make_unique_buffer<uint8_t>(chunk_size); // для подготовки пространства
  is.read(reinterpret_cast<char*>(bufferized.buffer.data.get()), chunk_size);
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(bufferized.crc_adler));
  bufferized.crc_adler = csc::from_be_to_system_endian(bufferized.crc_adler);
  return bufferized;
}
void check_for_chunk_errors(const csc::picture& image) {
  const csc::IHDR& header = cstd::get<csc::IHDR>(image.m_structured[0]);
  const csc::v_sections& sns = image.m_structured;
  const auto is_plte_type = [](const v_section& sn) { return cstd::holds_alternative<csc::PLTE>(sn); };

  const auto plte_pos = std::ranges::find_if(sns, is_plte_type);
  [[unlikely]] if (!cstd::holds_alternative<csc::IHDR>(sns.front()))
    throw cstd::domain_error("Блок IHDR не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (!cstd::holds_alternative<csc::IEND>(sns.back()))
    throw cstd::domain_error("Блок IEND не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (plte_pos == sns.cend() && header.color_type == e_color_type::indexed)
    throw cstd::domain_error("Для индексного изображения требуется палитра!");
  // [[unlikely]] if (image.m_image_data.empty()) - пока не пригодится
  //   throw cstd::domain_error("Данные изображения пусты. Файл, вероятно,
  //   поврежден!");
}

} // namespace csc
