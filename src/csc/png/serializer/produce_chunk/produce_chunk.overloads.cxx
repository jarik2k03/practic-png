module;
#include <algorithm>
#include <cstdint>
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
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::pHYs& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::hIST& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::cHRM& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::bKGD& s, csc::chunk& blob, const csc::IHDR& header) noexcept {
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::tIME& s, csc::chunk& blob) noexcept {
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::IHDR& s, csc::chunk& blob) noexcept {
  blob.resize(csc::IHDR::size());
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::PLTE& s, csc::chunk& blob, const csc::IHDR& header) noexcept {
  csc::buf_writer wrt(blob.data.data());
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(
    const csc::IDAT&,
    csc::chunk& blob,
    const csc::IHDR& header,
    const cstd::vector<uint8_t>& generic_image) noexcept {
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::IEND&, csc::chunk& ) noexcept {
  return csc::e_section_code::success;
}

} // namespace csc
