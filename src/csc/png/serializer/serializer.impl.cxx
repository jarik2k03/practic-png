module;
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <bits/allocator.h>
#include <cstdint>
module csc.png.serializer:impl;

import :utility;

import stl.string_view;
import stl.fstream;
import stl.stdexcept;
import stl.variant;
import stl.ios;
import stl.string;

import csc.png.serializer.produce_chunk.buf_writer;
import csc.png.serializer.produce_chunk;
import csc.png.serializer.produce_chunk.deflater;
import csc.png.commons.utility.memory_literals;
import csc.png.commons.utility.crc32;

export import csc.png.picture;
export import csc.png.serializer.produce_chunk.deflater.attributes;

import csc.png.commons.buffer_view;

namespace csc {
namespace png {

class serializer_impl {
 public:
  void do_serialize(
      std::string_view fp,
      const png::picture& i,
      png::e_compression_level l,
      int32_t m,
      int32_t wb,
      png::e_compression_strategy s);
};

void serializer_impl::do_serialize(
    std::string_view filepath,
    const png::picture& image,
    png::e_compression_level level,
    int32_t mem_level,
    int32_t win_bits,
    png::e_compression_strategy strategy) {
  using namespace png::memory_literals;
  std::ofstream png_fs;
  png_fs.open(filepath.data(), std::ios_base::binary);
  if (!png_fs.is_open())
    throw std::runtime_error("Не удалось открыть файл на запись!");

  png::write_png_signature_to_file(png_fs, image);
  // преобразование секций в чанки
  auto produce_chunk_and_write_to_file = [&png_fs](const auto& v_sec) {
    png::chunk raw_chunk;
    std::visit(png::f_produce_chunk(raw_chunk), v_sec);
    if (raw_chunk != png::chunk())
      png::write_chunk_to_ofstream(png_fs, raw_chunk);
  };
  const auto input_fragments = png::split_vector_to_fragments<png::u8buffer_view>(image.m_image_data, 256_kB);

  // кодирование изображения
  png::chunk raw_chunk = png::make_idat_templated_chunk();
  png::common_deflater z_stream(level, strategy, mem_level, win_bits);
  auto deflate_fragment_to_file = [&z_stream, &raw_chunk, &png_fs](png::u8buffer_view current) {
    z_stream.flush(current);
    do {
      z_stream.deflate(256_kB);
      raw_chunk.buffer.resize(std::ranges::distance(z_stream.value())); // обновляем границы буфера (без реаллокаций)
      std::ranges::copy(z_stream.value(), raw_chunk.buffer.begin()); // переносим данные из внутреннего в текущий
      raw_chunk.crc_adler = png::crc32_for_chunk(raw_chunk.chunk_name, raw_chunk.buffer); // контрольная сумма
      png::write_chunk_to_ofstream(png_fs, raw_chunk); // запись в файл
    } while (
        !z_stream.done()); // если сжатия нет, дефлейте придется делать обходами: 256kb read = 16kb + 16kb + ... write
  };
  // сериализация чанков в файл
  const auto range = std::ranges::subrange{image.m_structured.cbegin(), image.m_structured.cend() - 1};
  std::ranges::for_each(range, produce_chunk_and_write_to_file); // записываем всё, кроме IEND

  std::ranges::for_each(input_fragments, deflate_fragment_to_file); // сдуваем и толкаем в файл
  produce_chunk_and_write_to_file(image.m_structured.back()); // записываем IEND

  png_fs.close();
}

} // namespace png
} // namespace csc
