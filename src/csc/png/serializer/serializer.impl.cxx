module;
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <zlib.h>
module csc.png.serializer:impl;

import :utility;

import cstd.stl_wrap.string_view;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.ios;
import cstd.stl_wrap.iostream;
import cstd.stl_wrap.string;

export import csc.png.picture;
import csc.png.serializer.produce_chunk;
export import csc.png.serializer.produce_chunk.deflater;
import csc.png.serializer.produce_chunk.buf_writer;

import csc.png.commons.buffer_view;

namespace csc {

class serializer_impl {
 public:
  void do_serialize(cstd::string_view filepath, const csc::picture& image, csc::e_compression_level level);
};

void serializer_impl::do_serialize(
    cstd::string_view filepath,
    const csc::picture& image,
    csc::e_compression_level level) {
  cstd::ofstream png_fs;
  png_fs.open(filepath.data(), cstd::ios_base::binary);
  if (!png_fs.is_open())
    throw cstd::runtime_error("Не удалось открыть файл на запись!");

  png_fs.write(reinterpret_cast<const char*>(&image.start()),
               sizeof(csc::png_signature)); // пнг-подпись

  auto create_crc32 = [](const cstd::array<char, 4>& name, const csc::u8buffer& data) -> uint32_t {
    uint32_t crc = crc32(0ul, reinterpret_cast<const uint8_t*>(name.cbegin()), name.size());
    crc = crc32(crc, data.begin(), data.size());
    return crc;
  };
  auto create_chunk_and_write_to_file = [&](const auto& v_sec) {
    csc::chunk raw_chunk;
    cstd::visit(csc::f_produce_chunk(raw_chunk), v_sec);
    if (raw_chunk != csc::chunk())
      csc::write_chunk_to_ofstream(png_fs, raw_chunk);
  };
  csc::chunk raw_chunk;
  raw_chunk.chunk_name = cstd::array<char, 4>{'I', 'D', 'A', 'T'};
  raw_chunk.buffer = csc::make_buffer<uint8_t>(16384u);

  csc::deflater z_stream(level);
  const auto input_segments = csc::split_vector_to_chunks<csc::u8buffer_view>(image.m_image_data, 32768u);

  auto deflate_and_write_to_file = [&](csc::u8buffer_view current) {
    z_stream.flush(current);
    do {
      z_stream.deflate(32768u);
      raw_chunk.buffer.resize(std::ranges::distance(z_stream.value())); // обновляем границы буфера (без реаллокаций)
      std::ranges::copy(z_stream.value(), raw_chunk.buffer.begin()); // переносим данные из внутреннего в текущий
      raw_chunk.crc_adler = create_crc32(raw_chunk.chunk_name, raw_chunk.buffer); // контрольная сумма
      csc::write_chunk_to_ofstream(png_fs, raw_chunk); // запись в файл
    } while (!z_stream.done()); // если сжатия нет, дефлейте придется сделать два обхода: 32kb read = 16kb + 16kb write
  };

  const auto range = std::ranges::subrange{image.m_structured.cbegin(), image.m_structured.cend() - 1};
  std::ranges::for_each(range, create_chunk_and_write_to_file); // записываем всё, кроме IEND

  std::ranges::for_each(input_segments, deflate_and_write_to_file); // сдуваем и толкаем в файл
  create_chunk_and_write_to_file(image.m_structured.back()); // записываем IEND

  png_fs.close();
}

} // namespace csc
