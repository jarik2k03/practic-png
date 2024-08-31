module;
#include <cstdint>
module csc.png.serializer:utility;

import cstd.stl_wrap.fstream;
import csc.png.commons.chunk;
import csc.png.serializer.produce_chunk.buf_writer;
namespace csc {

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

} // namespace csc
