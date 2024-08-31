module;
#include <bits/move.h>
#include <cstdint>
export module csc.png.deserializer:impl;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.string;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.ios;

import :utility;

export import csc.png.picture;
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
    auto v_section = csc::init_section(chunk);
    const auto result = cstd::visit(csc::f_consume_chunk(chunk, image.m_structured), v_section);
    if (result != e_section_code::success) {
      const cstd::string err_msg = "Ошибка в представлении сектора: " + cstd::string(chunk.chunk_name.data(), 4);
      throw cstd::domain_error(err_msg);
    }
    image.m_structured.emplace_back(std::move(v_section));
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
