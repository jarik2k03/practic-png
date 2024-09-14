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
import cstd.stl_wrap.iostream;

import :utility;

export import csc.png.picture;

import csc.png.commons.chunk;
import csc.png.deserializer.consume_chunk;
import csc.png.deserializer.consume_chunk.inflater;

namespace csc {

class deserializer_impl {
 public:
  csc::picture do_deserialize(cstd::string_view filepath, bool ignore_checksum);
};

csc::picture deserializer_impl::do_deserialize(cstd::string_view filepath, bool ignore_checksum) {
  using cstd::operator+;
  using cstd::operator==;
  cstd::ifstream png_fs;
  csc::picture image;
  csc::common_inflater z_stream;
  png_fs.open(filepath.data(), cstd::ios_base::binary);
  if (!png_fs.is_open())
    throw cstd::runtime_error("Не существует файла в указанной директории!");
  csc::read_png_signature_from_file(png_fs, image);
  try {
    auto header_chunk = csc::read_chunk_from_ifstream(png_fs);
    consume_chunk_and_write_to_image(header_chunk, image), check_sum(header_chunk);
    image.m_image_data.reserve(csc::bring_image_size(cstd::get<IHDR>(image.m_structured.at(0))));
    while (png_fs.tellg() != -1) {
      auto chunk = csc::read_chunk_from_ifstream(png_fs);
      using cstd::operator==;
      if (chunk.chunk_name == cstd::array<char, 4>{'I', 'D', 'A', 'T'}) {
        inflate_fragment_to_image(chunk, image, z_stream);
        if (ignore_checksum == false)
          check_sum(chunk);
      } else {
        consume_chunk_and_write_to_image(chunk, image);
        if (ignore_checksum == false)
          check_sum(chunk);
      }
      [[unlikely]] if (chunk.chunk_name == cstd::array<char, 4>{'I', 'E', 'N', 'D'})
        break; // чтобы уж точно ничего не сломалось
    }
    png_fs.close();
  } catch (const cstd::runtime_error& e) {
    throw cstd::domain_error(cstd::string("Недесериализован блок данных (IDAT): ") + e.what());
  } catch (const cstd::domain_error& e) {
    throw cstd::domain_error(cstd::string("Недесериализован чанк: ") + e.what());
  } catch (const cstd::exception& e) {
    throw cstd::domain_error(cstd::string("Ошибка на этапе десериализации: ") + e.what());
  }
  try {
    csc::check_for_chunk_errors(image);
  } catch (const cstd::domain_error& e) {
    throw cstd::domain_error(cstd::string("PNG-изображение не прошло пост-проверку: ") + e.what());
  }
  return image;
}
} // namespace csc
