module;
#include <bits/move.h>
#include <bits/stl_construct.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cmath>
#include <cstring>
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
  void do_prepare_to_present_1(png::picture& deserialized);
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

void deserializer_impl::do_prepare_to_present_1(png::picture& deserialized) {
  auto& ihdr = deserialized.header();
  const png::IHDR src_ihdr = deserialized.header();
  auto& filtered = deserialized.m_image_data;

  auto channels_count = png::channels_count_from_color_type(ihdr.color_type);
  const auto ceil_pixel_bytesize = channels_count * static_cast<uint32_t>(std::ceilf(ihdr.bit_depth / 8.f));

  /* первая волна */
  constexpr const auto x_init = 0u, x_jump = 8u, y_init = 0u, y_jump = 8u;

  auto interlace_width = (src_ihdr.width - x_init + x_jump - 1) / x_jump;
  auto width_bytes = static_cast<uint32_t>(::ceilf(interlace_width * ceil_pixel_bytesize));
  auto interlace_height = (src_ihdr.height - y_init + y_jump - 1) / y_jump;
  std::vector<uint8_t> unfiltered_image(interlace_width * interlace_height * channels_count);

  png::unfilterer_1 decoder_1(filtered, width_bytes, ceil_pixel_bytesize);
  for (auto height = y_init; height < src_ihdr.height; height += y_jump) {
    const auto& prod = decoder_1.unfilter_line();

  }
  ihdr.width = interlace_width, ihdr.height = interlace_height;

  // теперь вместо фильтрованного изображения будет лежать нефильтрованное
  deserialized.m_image_data = std::move(unfiltered_image);
}

void deserializer_impl::do_prepare_to_present(png::picture& deserialized) {
  const auto& ihdr = deserialized.header();
  auto& filtered = deserialized.m_image_data;

  auto channels_count = png::channels_count_from_color_type(ihdr.color_type);
  const auto ceil_pixel_bytesize = channels_count * static_cast<uint32_t>(std::ceilf(ihdr.bit_depth / 8.f));
  const auto width_bytes = static_cast<uint32_t>(::ceilf(ihdr.width * ceil_pixel_bytesize));

  std::vector<uint8_t> unfiltered_image(width_bytes * ihdr.height);

  png::unfilterer_0 decoder(filtered, width_bytes, ceil_pixel_bytesize);
  for (auto write_offset = 0u; write_offset < width_bytes * ihdr.height; write_offset += width_bytes) {
    const auto& prod = decoder.unfilter_line();
    ::memcpy(unfiltered_image.data() + write_offset, prod.data(), prod.size());
  }
  // теперь вместо фильтрованного изображения будет лежать нефильтрованное
  deserialized.m_image_data = std::move(unfiltered_image);
}

} // namespace png
} // namespace csc
