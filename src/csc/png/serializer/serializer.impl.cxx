module;
#include <algorithm>
#include <cstdint>
module csc.png.serializer_lib:impl;

import csc.stl_wrap.string_view;
import csc.stl_wrap.string;
import csc.stl_wrap.vector;
import csc.stl_wrap.fstream;
import csc.stl_wrap.stdexcept;
import csc.stl_wrap.variant;
import csc.stl_wrap.ios;
import csc.stl_wrap.iostream;

export import csc.png.picture;
import csc.png.picture.sections.inflater;

namespace csc {

class serializer_impl {
 public:
  void do_serialize(csc::string_view filepath, const csc::picture& image);
};

void write_chunk_to_ofstream(csc::ofstream& os, const csc::chunk& bufferized) {
  // запись длины недесериализованного блока в файл
  const uint32_t chunk_size_be = csc::swap_endian(bufferized.data.size());
  os.write(reinterpret_cast<const char*>(&chunk_size_be), sizeof(uint32_t));
  // запись имени чанка
  os.write(reinterpret_cast<const char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись ранее сжатых (deflated) данных в файл
  os.write(reinterpret_cast<const char*>(bufferized.data.data()), bufferized.data.size());
  // запись контрольной суммы в файл
  const uint32_t crc_adler_be = csc::swap_endian(bufferized.crc_adler);
  os.write(reinterpret_cast<const char*>(&crc_adler_be), sizeof(uint32_t));
}

void serializer_impl::do_serialize(csc::string_view filepath, const csc::picture& image) {
  csc::ofstream png_fs;
  png_fs.open(filepath.data(), csc::ios_base::binary);
  if (!png_fs.is_open())
    throw csc::runtime_error("Не удалось открыть файл на запись!");

  png_fs.write(reinterpret_cast<const char*>(&image.start()), sizeof(csc::SUBSCRIBE)); // пнг-подпись

  // csc::deflater z_stream;
  for (const auto& section : image.m_structured) {
    // const auto chunk = csc::visit(csc::f_store_to_chunk(chunk, image.m_structured, z_stream, image.m_image_data),
    // section); csc::write_chunk_to_ofstream(png_fs, chunk);
  }

  png_fs.close();
}
} // namespace csc
