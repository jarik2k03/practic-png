module;
#include <algorithm>
#include <cstdint>
#include <ranges>
module csc.png.serializer:impl;

import cstd.stl_wrap.string_view;
import cstd.stl_wrap.string;
import cstd.stl_wrap.vector;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.ios;
import cstd.stl_wrap.iostream;

export import csc.png.picture;
import csc.png.serializer.produce_chunk;
import csc.png.serializer.produce_chunk.buf_writer;

namespace csc {

class serializer_impl {
 public:
  void do_serialize(cstd::string_view filepath, const csc::picture& image);
};

void write_chunk_to_ofstream(cstd::ofstream& os, const csc::chunk& bufferized) {
  // запись длины недесериализованного блока в файл
  const uint32_t chunk_size_be = csc::from_system_endian_to_be(bufferized.buffer.size);
  os.write(reinterpret_cast<const char*>(&chunk_size_be), sizeof(uint32_t));
  // запись имени чанка
  os.write(reinterpret_cast<const char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись ранее сжатых (deflated) данных в файл
  os.write(reinterpret_cast<const char*>(bufferized.buffer.data.get()), bufferized.buffer.size);
  // запись контрольной суммы в файл
  const uint32_t crc_adler_be = csc::from_system_endian_to_be(bufferized.crc_adler);
  os.write(reinterpret_cast<const char*>(&crc_adler_be), sizeof(uint32_t));
}

void serializer_impl::do_serialize(cstd::string_view filepath, const csc::picture& image) {
  cstd::ofstream png_fs;
  png_fs.open(filepath.data(), cstd::ios_base::binary);
  if (!png_fs.is_open())
    throw cstd::runtime_error("Не удалось открыть файл на запись!");

  png_fs.write(reinterpret_cast<const char*>(&image.start()), sizeof(csc::png_signature)); // пнг-подпись

  // csc::deflater z_stream;
  auto create_chunk_and_write_to_file = [&](const auto& v_sec) {
    csc::chunk raw_chunk;
    cstd::visit(csc::f_produce_chunk(raw_chunk), v_sec);
    csc::write_chunk_to_ofstream(png_fs, raw_chunk);
  };
  std::ranges::for_each(image.m_structured, create_chunk_and_write_to_file);

  png_fs.close();
}
} // namespace csc
