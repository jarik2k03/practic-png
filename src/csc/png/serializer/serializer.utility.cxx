module;
#include <cstdint>
#include <type_traits>
module csc.png.serializer:utility;

import csc.png.picture;
import cstd.stl_wrap.vector;
import cstd.stl_wrap.fstream;
import csc.png.commons.chunk;
import csc.png.serializer.produce_chunk.buf_writer;
import csc.png.commons.utility.memory_literals;

namespace csc {

void write_png_signature_to_file(cstd::ofstream& fs, const csc::picture& image) {
  fs.write(reinterpret_cast<const char*>(&image.start()), sizeof(csc::png_signature));
}

template <class Cont>
constexpr cstd::vector<Cont> split_vector_to_fragments(const cstd::vector<uint8_t>& generic, uint32_t bits) {
  cstd::vector<Cont> partitions;
  partitions.reserve(generic.size() / bits + 1);
  auto i = 0u;
  if (generic.size() >= bits) {
    for (; i < generic.size() - bits; i += bits) {
      const auto beg = generic.cbegin() + i, end = beg + bits;
      partitions.emplace_back(Cont(beg, end));
    }
  }
  partitions.emplace_back(Cont(generic.cbegin() + i, generic.cend()));
  return partitions;
}

csc::chunk make_idat_templated_chunk() {
  using namespace csc::memory_literals;
  csc::chunk newbie;
  newbie.chunk_name = cstd::array<char, 4>{'I', 'D', 'A', 'T'};
  newbie.buffer = csc::make_buffer<uint8_t>(16_kB);
  return newbie;
}

constexpr void write_chunk_to_ofstream(cstd::ofstream& os, const csc::chunk& bufferized) {
  // запись длины недесериализованного блока в файл
  const uint32_t chunk_size_be = csc::from_system_endian_to_be(bufferized.buffer.size());
  os.write(reinterpret_cast<const char*>(&chunk_size_be), sizeof(uint32_t));
  // запись имени чанка
  os.write(reinterpret_cast<const char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись ранее сжатых (deflated) данных в файл
  os.write(reinterpret_cast<const char*>(bufferized.buffer.data()), bufferized.buffer.size());
  // запись контрольной суммы в файл
  const uint32_t crc_adler_be = csc::from_system_endian_to_be(bufferized.crc_adler);
  os.write(reinterpret_cast<const char*>(&crc_adler_be), sizeof(uint32_t));
}

} // namespace csc
