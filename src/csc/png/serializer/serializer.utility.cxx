module;
#include <cstdint>
#include <type_traits>
module csc.png.serializer:utility;

import csc.png.picture;
import stl.vector;
import stl.fstream;
import csc.png.commons.chunk;
import csc.png.serializer.produce_chunk.buf_writer;
import csc.png.commons.utility.memory_literals;

namespace csc {
namespace png {

void write_png_signature_to_file(std::ofstream& fs, const png::picture& image) {
  fs.write(reinterpret_cast<const char*>(&image.start()), sizeof(png::png_signature));
}

template <class Cont>
constexpr std::vector<Cont> split_vector_to_fragments(const std::vector<uint8_t>& generic, uint32_t bits) {
  std::vector<Cont> partitions;
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

png::chunk make_idat_templated_chunk() {
  using namespace png::memory_literals;
  png::chunk newbie;
  newbie.chunk_name = std::array<char, 4>{'I', 'D', 'A', 'T'};
  newbie.buffer = png::make_buffer<uint8_t>(16_kB);
  return newbie;
}

constexpr void write_chunk_to_ofstream(std::ofstream& os, const png::chunk& bufferized) {
  // запись длины недесериализованного блока в файл
  const uint32_t chunk_size_be = png::from_system_endian_to_be(bufferized.buffer.size());
  os.write(reinterpret_cast<const char*>(&chunk_size_be), sizeof(uint32_t));
  // запись имени чанка
  os.write(reinterpret_cast<const char*>(&bufferized.chunk_name), sizeof(bufferized.chunk_name));
  // запись ранее сжатых (deflated) данных в файл
  os.write(reinterpret_cast<const char*>(bufferized.buffer.data()), bufferized.buffer.size());
  // запись контрольной суммы в файл
  const uint32_t crc_adler_be = png::from_system_endian_to_be(bufferized.crc_adler);
  os.write(reinterpret_cast<const char*>(&crc_adler_be), sizeof(uint32_t));
}

} // namespace png
} // namespace csc
