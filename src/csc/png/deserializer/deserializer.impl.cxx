module;
#include <bits/move.h>
#include <bits/stl_construct.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
export module csc.png.deserializer:impl;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.string;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.vector;
import cstd.stl_wrap.ios;

import :utility;

export import csc.png.picture;

import csc.png.commons.chunk;
import csc.png.commons.utility.crc32;
import csc.png.deserializer.consume_chunk;
import csc.png.deserializer.consume_chunk.inflater;
import csc.png.deserializer.consume_chunk.buf_reader;

namespace csc {

class deserializer_impl {
 public:
  csc::picture do_deserialize(cstd::string_view filepath);
};

csc::picture deserializer_impl::do_deserialize(cstd::string_view filepath) {
  using cstd::operator+;
  using cstd::operator==;
  cstd::ifstream png_fs;
  png_fs.open(filepath.data(), cstd::ios_base::binary);
  if (!png_fs.is_open())
    throw cstd::runtime_error("Не существует файла в указанной директории!");

  csc::picture image;
  png_fs.read(reinterpret_cast<char*>(&image.start()), sizeof(csc::png_signature));
  if (image.start() != csc::png_signature())
    throw cstd::domain_error("Ошибка чтения png. Это не PNG файл!");
  // чтение данных из файла и сохранение чанков в памяти
  cstd::vector<csc::chunk> saved_idat, saved_sections;
  do {
    auto chunk = csc::read_chunk_from_ifstream(png_fs);
    if (chunk.chunk_name == cstd::array<char, 4>{'I', 'D', 'A', 'T'})
      saved_idat.emplace_back(std::move(chunk));
    else
      saved_sections.emplace_back(std::move(chunk));
  } while (png_fs.peek() != -1);
  png_fs.close();
  // контрольные суммы
  auto check_sum = [](const auto& chunk) {
    [[unlikely]] if (chunk.crc_adler != csc::crc32_for_chunk(chunk.chunk_name, chunk.buffer)) {
      throw cstd::domain_error("ЦРЦ дизматчь: " + cstd::string(chunk.chunk_name.data(), 4));
    }
  };
  std::ranges::for_each(saved_sections, check_sum);
  std::ranges::for_each(saved_idat, check_sum);
  // преобразование чанков в секции
  auto consume_chunk_and_write_to_image = [&image](const auto& chunk) {
    auto v_section = csc::init_section(chunk);
    const auto result = cstd::visit(csc::f_consume_chunk(chunk, image.m_structured), v_section);
    if (result != e_section_code::success)
      throw cstd::domain_error(csc::generate_section_error_message(result, chunk.chunk_name));
    image.m_structured.emplace_back(std::move(v_section));
  };
  std::ranges::for_each(saved_sections, consume_chunk_and_write_to_image);
  // декодирование изображения
  csc::common_inflater z_stream;
  image.m_image_data.reserve(csc::bring_image_size(image.m_header));
  auto inflate_fragment_to_image = [&image, &z_stream](const auto& chunk) {
    z_stream.flush(chunk.buffer);
    do {
      z_stream.inflate();
      std::ranges::copy(z_stream.value(), std::back_inserter(image.m_image_data));
    } while (!z_stream.done());
  };
  try {
    std::ranges::for_each(saved_idat, inflate_fragment_to_image);
  } catch (const cstd::runtime_error& e) {
    throw cstd::domain_error(cstd::string("Недесериализован блок данных (IDAT): ") + e.what());
  }

  // проверка правильности расположения секторов
  try {
    csc::check_for_chunk_errors(image);
  } catch (const cstd::domain_error& e) {
    throw cstd::domain_error(cstd::string("PNG-изображение не прошло проверку: ") + e.what());
  }
  return image;
}
} // namespace csc
