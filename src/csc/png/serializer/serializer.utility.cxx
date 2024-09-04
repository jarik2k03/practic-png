module;
#include <cstdint>
#include <type_traits>
module csc.png.serializer:utility;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.fstream;
import csc.png.commons.chunk;
import csc.png.serializer.produce_chunk.buf_writer;

namespace csc {

template <class Cont>
cstd::vector<Cont> split_vector_to_chunks(const cstd::vector<uint8_t>& generic, uint32_t bits) {
  cstd::vector<Cont> partitions;
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

void write_chunk_to_ofstream(cstd::ofstream& os, const csc::chunk& bufferized) {
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
