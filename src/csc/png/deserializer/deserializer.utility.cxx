module;
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
module csc.png.deserializer:utility;

import csc.png.deserializer.consume_chunk.buf_reader;
import csc.png.commons.chunk;
import stl.array;
import stl.string;
import stl.fstream;
import stl.stdexcept;
import stl.optional;

import csc.png.picture;
import csc.png.deserializer.consume_chunk;
import csc.png.commons.utility.crc32;
import csc.png.deserializer.consume_chunk.inflater;
import csc.png.deserializer.consume_chunk.buf_reader;

namespace csc {
namespace png {

constexpr std::string generate_section_error_message(png::e_section_code sec, std::array<char, 4> name) noexcept {
  using namespace std::string_literals;
  using enum png::e_section_code;
  switch (sec) {
    case error:
      return "Ошибка в представлении сектора: "s + std::string(name.data(), 4);
    case success:
      return ""s;
    case depends_ihdr:
      return "Секция "s + std::string(name.data(), 4) + " требует наличия зависимого чанка IHDR";
  };
  return "Неизвестная ошибка секции!"s;
}

constexpr uint8_t channels_count_from_color_type(png::e_color_type t) {
  using enum png::e_color_type;
  switch (t) {
    case rgba:
      return 4u;
    case rgb:
      return 3u;
    case bw:
      return 1u;
    case bwa:
      return 2u;
    case indexed:
      return 1u;
    default:
      return 0u;
  }
  return 0u;
}

constexpr uint32_t bring_unfiltered_image_size(const png::IHDR& header) {
  const float pixel_size = header.bit_depth / 8.f;
  const uint32_t channels = png::channels_count_from_color_type(header.color_type);
  // (длина * высота * размер * общий размер пикселя
  return static_cast<uint32_t>(header.width * header.height * pixel_size * channels);
}

constexpr uint32_t bring_filtered_image_size(const png::IHDR& header, float reserve_coefficient) {
  const float pixel_size = header.bit_depth / 8.f;
  const uint32_t channels = png::channels_count_from_color_type(header.color_type);
  // (длина * высота * размер * общий размер пикселя + байт фильтрации на каждую строку) + 10% резерв
  return static_cast<uint32_t>(
      (header.width * header.height * pixel_size * channels + (header.height * 1)) * reserve_coefficient);
}

constexpr std::optional<png::v_section> init_section(const png::chunk& ch) {
  if (ch.chunk_name == std::array<char, 4>{'I', 'H', 'D', 'R'})
    return png::v_section(png::IHDR());
  else if (ch.chunk_name == std::array<char, 4>{'P', 'L', 'T', 'E'})
    return png::v_section(png::PLTE());
  else if (ch.chunk_name == std::array<char, 4>{'I', 'E', 'N', 'D'})
    return png::v_section(png::IEND());
  else if (ch.chunk_name == std::array<char, 4>{'b', 'K', 'G', 'D'})
    return png::v_section(png::bKGD());
  else if (ch.chunk_name == std::array<char, 4>{'t', 'I', 'M', 'E'})
    return png::v_section(png::tIME());
  else if (ch.chunk_name == std::array<char, 4>{'c', 'H', 'R', 'M'})
    return png::v_section(png::cHRM());
  else if (ch.chunk_name == std::array<char, 4>{'g', 'A', 'M', 'A'})
    return png::v_section(png::gAMA());
  else if (ch.chunk_name == std::array<char, 4>{'h', 'I', 'S', 'T'})
    return png::v_section(png::hIST());
  else if (ch.chunk_name == std::array<char, 4>{'p', 'H', 'Y', 's'})
    return png::v_section(png::pHYs());
  else if (ch.chunk_name == std::array<char, 4>{'t', 'R', 'N', 'S'})
    return png::v_section(png::tRNS());
  else
    return std::nullopt; // не распознаётся
}

png::chunk read_chunk_from_ifstream(std::ifstream& is) {
  png::chunk bufferized;
  // длина недесериализованного блока в чанке
  uint32_t chunk_size;
  is.read(reinterpret_cast<char*>(&chunk_size), sizeof(chunk_size));
  chunk_size = png::from_be_to_system_endian(chunk_size);
  // имя чанка
  is.read(reinterpret_cast<char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись недесериализованного блока в чанк
  bufferized.buffer = png::make_buffer<uint8_t>(chunk_size); // для подготовки пространства
  is.read(reinterpret_cast<char*>(bufferized.buffer.data()), chunk_size);
  // запись контрольной суммы в чанк
  is.read(reinterpret_cast<char*>(&bufferized.crc_adler), sizeof(bufferized.crc_adler));
  bufferized.crc_adler = png::from_be_to_system_endian(bufferized.crc_adler);
  return bufferized;
}

// функции-фабрикаторы top-level
void read_png_signature_from_file(std::ifstream& fs, png::picture& image) {
  fs.read(reinterpret_cast<char*>(&image.start()), sizeof(png::png_signature));
  if (image.start() != png::png_signature())
    throw std::domain_error("Ошибка чтения png. Это не PNG файл!");
}
constexpr void consume_chunk_and_write_to_image(const png::chunk& chunk, png::picture& image) {
  auto v_section = png::init_section(chunk);
  if (v_section == std::nullopt)
    return; // нет ничего ошибочного в том, что секцию не распознаёт программа
  const auto result = std::visit(png::f_consume_chunk(chunk, image.m_structured), *v_section);
  if (result != e_section_code::success)
    throw std::domain_error(png::generate_section_error_message(result, chunk.chunk_name));
  image.m_structured.emplace_back(std::move(*v_section));
}

void check_sum(const png::chunk& chunk) {
  [[unlikely]] if (chunk.crc_adler != png::crc32_for_chunk(chunk.chunk_name, chunk.buffer)) {
    throw std::domain_error("ЦРЦ дизматчь: " + std::string(chunk.chunk_name.data(), 4));
  }
}
void inflate_fragment_to_image(const auto& chunk, png::picture& image, png::common_inflater& inflater) {
  inflater.flush(chunk.buffer);
  do {
    inflater.inflate();
    std::ranges::copy(inflater.value(), std::back_inserter(image.m_image_data));
  } while (!inflater.done());
}
constexpr void check_for_chunk_errors(const png::picture& image) {
  const png::v_sections& sns = image.m_structured;
  const auto is_plte_type = [](const v_section& sn) { return std::holds_alternative<png::PLTE>(sn); };
  const auto plte_pos = std::ranges::find_if(sns, is_plte_type);
  [[unlikely]] if (sns.empty() || image.m_image_data.empty())
    throw std::range_error("Изображение не содержит никаких чанков!");
  [[unlikely]] if (!std::holds_alternative<png::IHDR>(sns.front()))
    throw std::domain_error("Блок IHDR не найден. Файл поврежден!");
  [[unlikely]] if (!std::holds_alternative<png::IEND>(sns.back()))
    throw std::domain_error("Блок IEND не найден. Файл поврежден!");
  const png::IHDR& header = std::get<png::IHDR>(image.m_structured.at(0));
  [[unlikely]] if (plte_pos == sns.cend() && header.color_type == e_color_type::indexed)
    throw std::domain_error("Для индексного изображения требуется палитра!");
}

} // namespace png
} // namespace csc
