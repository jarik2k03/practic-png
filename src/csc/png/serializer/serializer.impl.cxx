module;
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
module csc.png.serializer:impl;

import :utility;

import cstd.stl_wrap.string_view;
import cstd.stl_wrap.fstream;
import cstd.stl_wrap.stdexcept;
import cstd.stl_wrap.variant;
import cstd.stl_wrap.ios;

export import csc.png.picture;
import csc.png.serializer.produce_chunk;
// import csc.png.serializer.produce_chunk.deflater;
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

  // csc::deflater z_stream;
  auto create_chunk_and_write_to_file = [&](const auto& v_sec) {
    csc::chunk raw_chunk;
    cstd::visit(csc::f_produce_chunk(raw_chunk), v_sec);
    if (raw_chunk != csc::chunk())
      csc::write_chunk_to_ofstream(png_fs, raw_chunk);
  };
  std::ranges::for_each(image.m_structured, create_chunk_and_write_to_file);

  png_fs.close();
}
} // namespace csc
