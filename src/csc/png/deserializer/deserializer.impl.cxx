module;
#include <bits/move.h>
#include <bits/stl_construct.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <zlib.h>
#include <cstdint>
export module csc.png.deserializer:impl;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.string;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.vector;
import cstd.stl_wrap.ios;
import cstd.stl_wrap.iostream; // отлаживал

import :utility;

export import csc.png.picture;

import csc.png.commons.chunk;
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
  csc::picture image;
  if (!png_fs.is_open())
    throw cstd::runtime_error("Не существует файла в указанной директории!");

  png_fs.read(reinterpret_cast<char*>(&image.start()), sizeof(csc::png_signature));
  if (image.start() != csc::png_signature())
    throw cstd::runtime_error("Ошибка чтения png - предзаголовочная подпись не проинициализирована!");

  cstd::vector<csc::chunk> saved_idat;
  do {
    auto chunk = csc::read_chunk_from_ifstream(png_fs);
    auto v_section = csc::init_section(chunk);
    const auto result = cstd::visit(csc::f_consume_chunk(chunk, image.m_structured), v_section);
    if (result != e_section_code::success) {
      const cstd::string err_msg = "Ошибка в представлении сектора: " + cstd::string(chunk.chunk_name.data(), 4);
      throw cstd::domain_error(err_msg);
    }
    if (chunk.chunk_name != cstd::array<char, 4>{'I', 'D', 'A', 'T'})
      image.m_structured.emplace_back(std::move(v_section));
    else
      saved_idat.emplace_back(std::move(chunk));
  } while (png_fs.peek() != -1);
  // декодирование изображения
  using cstd::operator<<;
  csc::inflater z_stream;
  image.m_image_data.reserve(csc::f_calc_image_size(image.m_header));
  auto decode_with_inflater = [&image, &z_stream](const auto& chunk) {
    z_stream.set_compressed_buffer(chunk.buffer);
    do {
      z_stream.inflate(Z_NO_FLUSH);
      // cstd::cout << "Инфляция: " << std::ranges::distance(z_stream.value()) << '\n';
      std::ranges::copy(z_stream.value(), std::back_inserter(image.m_image_data));
    } while (!z_stream.done());
  };
  try {
    std::ranges::for_each(saved_idat, decode_with_inflater);
  } catch (const cstd::runtime_error& e) {
    using namespace cstd::string_literals;
    throw cstd::runtime_error("Недесериализован блок данных (IDAT): "_s + e.what());
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
