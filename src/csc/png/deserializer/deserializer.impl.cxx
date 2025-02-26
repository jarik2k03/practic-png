module;
#include <bits/move.h>
#include <bits/stl_construct.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cmath>
#include <cstdint>
export module csc.png.deserializer:impl;
import stl.string_view;
import stl.string;
import stl.fstream;
import stl.stdexcept;
import stl.variant;
import stl.vector;
import stl.ios;
import stl.iostream;

import :utility;

export import csc.png.picture;

import csc.png.commons.chunk;
import csc.png.deserializer.unfilterer;
import csc.png.deserializer.consume_chunk;
import csc.png.deserializer.consume_chunk.inflater;

namespace csc {
namespace png {

class deserializer_impl {
 public:
  png::picture do_deserialize(std::string_view filepath, bool ignore_checksum);
  void do_prepare_to_present(png::picture& deserialized);
};

png::picture deserializer_impl::do_deserialize(std::string_view filepath, bool ignore_checksum) {
  std::ifstream png_fs;
  png::picture image;
  png::common_inflater z_stream;
  png_fs.open(filepath.data(), std::ios_base::binary);
  if (!png_fs.is_open())
    throw std::runtime_error("Не существует файла в указанной директории!");
  png::read_png_signature_from_file(png_fs, image);
  try {
    auto header_chunk = png::read_chunk_from_ifstream(png_fs);
    png::consume_chunk_and_write_to_image(header_chunk, image), png::check_sum(header_chunk);
    image.m_image_data.reserve(png::bring_filtered_image_size(image.header(), 1.1f));
    while (png_fs.tellg() != -1) {
      auto chunk = png::read_chunk_from_ifstream(png_fs);

      if (chunk.chunk_name == std::array<char, 4>{'I', 'D', 'A', 'T'}) {
        png::inflate_fragment_to_image(chunk, image, z_stream);
        if (ignore_checksum == false)
          check_sum(chunk);
      } else {
        png::consume_chunk_and_write_to_image(chunk, image);
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
    png::check_for_chunk_errors(image);
  } catch (const std::domain_error& e) {
    throw std::domain_error(std::string("PNG-изображение не прошло пост-проверку: ") + e.what());
  }
  return image;
}

void deserializer_impl::do_prepare_to_present(png::picture& deserialized) {
  const auto& ihdr = deserialized.header();
  auto& filtered = deserialized.m_image_data;

  auto channels_count = png::channels_count_from_color_type(ihdr.color_type);
  const auto ceil_pixel_bytesize = channels_count * static_cast<uint32_t>(std::ceilf(ihdr.bit_depth / 8.f));
  const auto width_bytes = static_cast<uint32_t>(::ceilf(ihdr.width * ceil_pixel_bytesize));

  std::vector<uint8_t> unfiltered_image(width_bytes * ihdr.height);

  switch (ihdr.bit_depth) {
    case 1u:
      filtered = png::convert_from_1bit_to_8bit(filtered, width_bytes, ihdr.height);
      break;
    case 2u:
      filtered = png::convert_from_2bit_to_8bit(filtered, width_bytes, ihdr.height);
      break;
    case 4u:
      filtered = png::convert_from_4bit_to_8bit(filtered, width_bytes, ihdr.height);
      break;
    default:
      break;
  }
  png::unfilterer decoder({filtered.begin(), filtered.end()}, unfiltered_image, width_bytes, ceil_pixel_bytesize);
  for (auto height = 0u; height < ihdr.height; ++height) {
    decoder.unfilter_line();
  }
  // теперь вместо фильтрованного изображения будет лежать нефильтрованное
  deserialized.m_image_data = std::move(unfiltered_image);
}

} // namespace png
} // namespace csc
