module;
#include <algorithm>
#include <bits/stl_iterator.h>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <ranges>
module csc.png.serializer.produce_chunk:overloads;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.stdexcept;
import :utility;

// export import csc.png.serializer.produce_chunk.inflater;
export import csc.png.serializer.produce_chunk.buf_writer;
export import csc.png.picture.sections;
export import csc.png.commons.chunk;

namespace csc {

csc::e_section_code produce_chunk(const csc::gAMA& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.buffer.data.get());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::pHYs& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.buffer.data.get());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::hIST& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.buffer.data.get());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::cHRM& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.buffer.data.get());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::bKGD& s, csc::chunk& blob, const csc::IHDR& header) noexcept {
  csc::buf_writer wrt(blob.buffer.data.get());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::tIME& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.buffer.data.get());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::IHDR& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'I', 'H', 'D', 'R'};

  blob.buffer = csc::make_unique_buffer<uint8_t>(IHDR::size());
  csc::buf_writer wrt(blob.buffer.data.get());
  wrt.write(s.width), wrt.write(s.height), wrt.write(s.bit_depth);
  wrt.write(static_cast<uint8_t>(s.color_type)), wrt.write(static_cast<uint8_t>(s.compression));
  wrt.write(static_cast<uint8_t>(s.filter)), wrt.write(static_cast<uint8_t>(s.interlace));
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::PLTE& s, csc::chunk& blob, const csc::IHDR& header) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'P', 'L', 'T', 'E'};

  blob.buffer = csc::make_unique_buffer<uint8_t>(s.size());
  csc::buf_writer wrt(blob.buffer.data.get());
  for (const auto& unit : s.full_palette) {
    wrt.write(unit.r), wrt.write(unit.g), wrt.write(unit.b);
  }
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(
    const csc::IDAT&,
    csc::chunk& blob,
    const csc::IHDR& header,
    const cstd::vector<uint8_t>& generic_image) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'I', 'D', 'A', 'T'};
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::IEND&, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'I', 'E', 'N', 'D'};
  return csc::e_section_code::success;
}

} // namespace csc
