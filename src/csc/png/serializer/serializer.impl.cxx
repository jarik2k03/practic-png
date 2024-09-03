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

export import csc.png.picture;
import csc.png.serializer.produce_chunk;
import csc.png.serializer.produce_chunk.deflater;
import csc.png.serializer.produce_chunk.buf_writer;

namespace csc {

class serializer_impl {
 public:
  void do_serialize(cstd::string_view filepath, const csc::picture& image);
};

void serializer_impl::do_serialize(cstd::string_view filepath, const csc::picture& image) {
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
  auto create_deflation_and_write_to_file = [&]() {
    csc::chunk raw_chunk;
    raw_chunk.chunk_name = cstd::array<char, 4>{'I', 'D', 'A', 'T'};
    raw_chunk.buffer = csc::make_buffer<uint8_t>(8_kB);

    csc::deflater z_stream;
    z_stream.set_decompressed_buffer(image.m_image_data);

    using cstd::operator<<;
    do {
      z_stream.deflate(Z_FINISH); // нет необходимости переключать входной буфер. Он один (image.m_image_data)
      raw_chunk.buffer.resize(std::ranges::distance(z_stream.value())); // обновляем границы буфера (без реаллокаций)
      cstd::cout << "deflated: " << raw_chunk.buffer.size() << '\n';
      std::ranges::copy(z_stream.value(), raw_chunk.buffer.data()); // переносим данные из внутреннего в текущий
      raw_chunk.crc_adler = create_crc32(raw_chunk.chunk_name, raw_chunk.buffer); // контрольная сумма
      csc::write_chunk_to_ofstream(png_fs, raw_chunk); // запись в файл
    } while (!z_stream.done());
  };

  const auto range = std::ranges::subrange{image.m_structured.cbegin(), image.m_structured.cend() - 1};
  std::ranges::for_each(range, create_chunk_and_write_to_file); // записываем всё, кроме IEND
  create_deflation_and_write_to_file();
  create_chunk_and_write_to_file(image.m_structured.back()); // записываем IEND

  png_fs.close();
}
} // namespace csc
