module;
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <cmath>
module csc.png.deserializer:utility;

import csc.png.deserializer.consume_chunk.buf_reader;
import csc.png.commons.chunk;
import cstd.stl_wrap.array;
import cstd.stl_wrap.string;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;

import csc.png.picture;
import csc.png.deserializer.consume_chunk;
import csc.png.commons.utility.crc32;
import csc.png.deserializer.consume_chunk.inflater;
import csc.png.deserializer.consume_chunk.buf_reader;

namespace csc {

constexpr cstd::string generate_section_error_message(csc::e_section_code sec, cstd::array<char, 4> name) noexcept {
  using cstd::operator+;
  using namespace cstd::string_literals;
  using enum csc::e_section_code;
  switch (sec) {
    case error:
      return "Ошибка в представлении сектора: "_s + cstd::string(name.data(), 4);
    case success:
      return ""_s;
    case depends_ihdr:
      return "Секция "_s + cstd::string(name.data(), 4) + " требует наличия зависимого чанка IHDR";
  };
  return "Неизвестная ошибка секции!"_s;
}

constexpr uint8_t pixel_size_from_color_type(csc::e_color_type t) {
  using enum csc::e_color_type;
  switch (t) {
    case rgba:
      return 4u;
    case rgb:
      return 3u;
    case bw:
      return 2u;
    case bwa:
      return 3u;
    case indexed:
      return 1u;
    default:
      return 0u;
  }
  return 0u;
}

constexpr uint32_t bring_image_size(const csc::IHDR& header) {
  const float pixel_size = header.bit_depth / 8.f;
  const uint32_t channels = csc::pixel_size_from_color_type(header.color_type);
  // (длина * высота * размер * общий размер пикселя + байт на каждую строку) + 10% резерв
  return static_cast<uint32_t>((header.width * header.height * pixel_size * channels + (header.height * 1)) * 1.10f);
};

constexpr csc::v_section init_section(const csc::chunk& ch) {
  using cstd::operator==;
  if (ch.chunk_name == cstd::array<char, 4>{'I', 'H', 'D', 'R'})
    return csc::v_section(csc::IHDR());
  else if (ch.chunk_name == cstd::array<char, 4>{'P', 'L', 'T', 'E'})
    return csc::v_section(csc::PLTE());
  else if (ch.chunk_name == cstd::array<char, 4>{'I', 'E', 'N', 'D'})
    return csc::v_section(csc::IEND());
  else if (ch.chunk_name == cstd::array<char, 4>{'b', 'K', 'G', 'D'})
    return csc::v_section(csc::bKGD());
  else if (ch.chunk_name == cstd::array<char, 4>{'t', 'I', 'M', 'E'})
    return csc::v_section(csc::tIME());
  else if (ch.chunk_name == cstd::array<char, 4>{'c', 'H', 'R', 'M'})
    return csc::v_section(csc::cHRM());
  else if (ch.chunk_name == cstd::array<char, 4>{'g', 'A', 'M', 'A'})
    return csc::v_section(csc::gAMA());
  else if (ch.chunk_name == cstd::array<char, 4>{'h', 'I', 'S', 'T'})
    return csc::v_section(csc::hIST());
  else if (ch.chunk_name == cstd::array<char, 4>{'p', 'H', 'Y', 's'})
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
  bufferized.buffer = csc::make_buffer<uint8_t>(chunk_size); // для подготовки пространства
  is.read(reinterpret_cast<char*>(bufferized.buffer.data()), chunk_size);
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(bufferized.crc_adler));
  bufferized.crc_adler = csc::from_be_to_system_endian(bufferized.crc_adler);
  return bufferized;
}

// функции-фабрикаторы top-level
void read_png_signature_from_file(cstd::ifstream& fs, csc::picture& image) {
  fs.read(reinterpret_cast<char*>(&image.start()), sizeof(csc::png_signature));
  if (image.start() != csc::png_signature())
    throw cstd::domain_error("Ошибка чтения png. Это не PNG файл!");
}
constexpr void consume_chunk_and_write_to_image(const csc::chunk& chunk, csc::picture& image) {
  auto v_section = csc::init_section(chunk);
  const auto result = cstd::visit(csc::f_consume_chunk(chunk, image.m_structured), v_section);
  if (result != e_section_code::success)
    throw cstd::domain_error(csc::generate_section_error_message(result, chunk.chunk_name));
  image.m_structured.emplace_back(std::move(v_section));
}

void check_sum(const csc::chunk& chunk) {
  using cstd::operator+;
  [[unlikely]] if (chunk.crc_adler != csc::crc32_for_chunk(chunk.chunk_name, chunk.buffer)) {
    throw cstd::domain_error("ЦРЦ дизматчь: " + cstd::string(chunk.chunk_name.data(), 4));
  }
}
void inflate_fragment_to_image(const auto& chunk, csc::picture& image, csc::common_inflater& inflater) {
  inflater.flush(chunk.buffer);
  do {
    inflater.inflate();
    std::ranges::copy(inflater.value(), std::back_inserter(image.m_image_data));
  } while (!inflater.done());
}
constexpr void check_for_chunk_errors(const csc::picture& image) {
  const csc::v_sections& sns = image.m_structured;
  const auto is_plte_type = [](const v_section& sn) { return cstd::holds_alternative<csc::PLTE>(sn); };
  const auto plte_pos = std::ranges::find_if(sns, is_plte_type);
  [[unlikely]] if (sns.empty() || image.m_image_data.empty())
    throw cstd::range_error("Изображение не содержит никаких чанков!");
  [[unlikely]] if (!cstd::holds_alternative<csc::IHDR>(sns.front()))
    throw cstd::domain_error("Блок IHDR не найден. Файл, вероятно, поврежден!");
  [[unlikely]] if (!cstd::holds_alternative<csc::IEND>(sns.back()))
    throw cstd::domain_error("Блок IEND не найден. Файл, вероятно, поврежден!");
  const csc::IHDR& header = cstd::get<csc::IHDR>(image.m_structured.at(0));
  [[unlikely]] if (plte_pos == sns.cend() && header.color_type == e_color_type::indexed)
    throw cstd::domain_error("Для индексного изображения требуется палитра!");
}

} // namespace csc
