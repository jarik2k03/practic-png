module;
#include <bits/move.h>
#include <bits/stl_construct.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
export module csc.png.deserializer:impl;
import stl.stl_wrap.string_view;
import stl.stl_wrap.string;
import stl.stl_wrap.fstream;
import stl.stl_wrap.stdexcept;
import stl.stl_wrap.variant;
import stl.stl_wrap.vector;
import stl.stl_wrap.ios;
import stl.stl_wrap.iostream;

import :utility;

export import csc.png.picture;

import csc.png.commons.chunk;
import csc.png.deserializer.consume_chunk;
import csc.png.deserializer.consume_chunk.inflater;

namespace csc {

class deserializer_impl {
 public:
  csc::picture do_deserialize(std::string_view filepath, bool ignore_checksum);
};

csc::picture deserializer_impl::do_deserialize(std::string_view filepath, bool ignore_checksum) {
  using std::operator+;
  using std::operator==;
  std::ifstream png_fs;
  csc::picture image;
  csc::common_inflater z_stream;
  png_fs.open(filepath.data(), std::ios_base::binary);
  if (!png_fs.is_open())
    throw std::runtime_error("Не существует файла в указанной директории!");
  csc::read_png_signature_from_file(png_fs, image);
  try {
    auto header_chunk = csc::read_chunk_from_ifstream(png_fs);
    consume_chunk_and_write_to_image(header_chunk, image), check_sum(header_chunk);
    image.m_image_data.reserve(csc::bring_image_size(std::get<IHDR>(image.m_structured.at(0))));
    while (png_fs.tellg() != -1) {
      auto chunk = csc::read_chunk_from_ifstream(png_fs);
      using std::operator==;
      if (chunk.chunk_name == std::array<char, 4>{'I', 'D', 'A', 'T'}) {
        inflate_fragment_to_image(chunk, image, z_stream);
        if (ignore_checksum == false)
          check_sum(chunk);
      } else {
        consume_chunk_and_write_to_image(chunk, image);
        if (ignore_checksum == false)
          check_sum(chunk);
      }
      [[unlikely]] if (chunk.chunk_name == std::array<char, 4>{'I', 'E', 'N', 'D'})
        break; // чтобы уж точно ничего не сломалось
    }
    png_fs.close();
  } catch (const std::runtime_error& e) {
    throw std::domain_error(std::string("Недесериализован блок данных (IDAT): ") + e.what());
  } catch (const std::domain_error& e) {
    throw std::domain_error(std::string("Недесериализован чанк: ") + e.what());
  } catch (const std::exception& e) {
    throw std::domain_error(std::string("Ошибка на этапе десериализации: ") + e.what());
  }
  try {
    csc::check_for_chunk_errors(image);
  } catch (const std::domain_error& e) {
    throw std::domain_error(std::string("PNG-изображение не прошло пост-проверку: ") + e.what());
  }
  return image;
}
} // namespace csc
