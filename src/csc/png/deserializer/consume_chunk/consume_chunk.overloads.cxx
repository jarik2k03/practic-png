module;
#include <algorithm>
#include <cstdint>
#include <ranges>
module csc.png.deserializer.consume_chunk:overloads;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.stdexcept;
export import cstd.stl_wrap.variant;

#ifndef NDEBUG
import cstd.stl_wrap.iostream;
#endif

export import csc.png.deserializer.consume_chunk.inflater;
export import csc.png.picture.sections;
export import csc.png.commons.chunk;
export import csc.png.commons.utils;

namespace csc {
// вспомогательно для IDAT
constexpr uint32_t pixel_size_from_color_type(csc::e_color_type t) {
  using enum csc::e_color_type;
  switch (t) {
    case rgba:
      return 4u;
    case rgb:
      return 3u;
    case bw:
      return 2u;
    case bwa:
      return 3u;
    case indexed:
      return 1u;
    default:
      return 0u;
  }
}

csc::section_code_t consume_chunk(csc::bKGD& s, const csc::chunk& blob, const csc::IHDR& header) noexcept {
  uint32_t buf_pos = 0u;
  using enum csc::e_color_type;
  const auto& type = header.color_type;
  if ((type == rgb || type == rgba) && header.bit_depth == 8) {
    uint16_t r16, g16, b16;
    csc::read_var_from_vector_swap(r16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    csc::read_var_from_vector_swap(g16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    csc::read_var_from_vector_swap(b16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    s.color = csc::rgb8{static_cast<uint8_t>(r16), static_cast<uint8_t>(g16), static_cast<uint8_t>(b16)};
    s.color_type = csc::e_pixel_view_id::rgb8;
  } else if ((type == rgb || type == rgba) && header.bit_depth == 16) {
    uint16_t r16, g16, b16;
    csc::read_var_from_vector_swap(r16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    csc::read_var_from_vector_swap(g16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    csc::read_var_from_vector_swap(b16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    s.color = csc::rgb16{r16, g16, b16}, s.color_type = csc::e_pixel_view_id::rgb16;
  } else if ((type == bw || type == bwa) && header.bit_depth <= 8) {
    uint16_t bw16;
    csc::read_var_from_vector_swap(bw16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    s.color = csc::bw8{static_cast<uint8_t>(bw16)}, s.color_type = csc::e_pixel_view_id::bw8;
  } else if ((type == bw || type == bwa) && header.bit_depth == 16) {
    uint16_t bw16;
    csc::read_var_from_vector_swap(bw16, buf_pos, blob.data), buf_pos += sizeof(uint16_t);
    s.color = csc::bw16{bw16}, s.color_type = csc::e_pixel_view_id::bw16;
  } else if (type == indexed) {
    uint8_t idx8;
    csc::read_var_from_vector(idx8, buf_pos, blob.data), buf_pos += sizeof(uint8_t);
    s.color = csc::plte_index{idx8}, s.color_type = csc::e_pixel_view_id::plte_index;
  }
  return csc::section_code_t::success;
}

} // namespace csc


namespace csc {

csc::section_code_t consume_chunk(csc::IHDR& s, const csc::chunk& blob) noexcept {
  uint32_t buf_pos = 0u; // для header не требуются зависимые сектора
  csc::read_var_from_vector_swap(s.width, buf_pos, blob.data), buf_pos += sizeof(s.width);
  csc::read_var_from_vector_swap(s.height, buf_pos, blob.data), buf_pos += sizeof(s.height);
  csc::read_var_from_vector(s.bit_depth, buf_pos, blob.data), buf_pos += sizeof(s.bit_depth);
  csc::read_var_from_vector(s.color_type, buf_pos, blob.data), buf_pos += sizeof(s.color_type);
  csc::read_var_from_vector(s.compression, buf_pos, blob.data), buf_pos += sizeof(s.compression);
  csc::read_var_from_vector(s.filter, buf_pos, blob.data), buf_pos += sizeof(s.filter);
  csc::read_var_from_vector(s.interlace, buf_pos, blob.data), buf_pos += sizeof(s.interlace);
  s.crc_adler = blob.crc_adler;
  return csc::section_code_t::success;
}

csc::section_code_t consume_chunk(csc::PLTE& s, const csc::chunk& blob, const csc::IHDR& header) noexcept {
  if (blob.data.size() % 3 != 0 || blob.data.size() > (256 * 3))
    return csc::section_code_t::error;
  if (header.color_type == e_color_type::indexed && blob.data.size() / 3 > (1 << header.bit_depth))
    return csc::section_code_t::error;
  s.full_palette.reserve(256u);

  for (uint32_t pos = 0u; pos < blob.data.size(); pos += 3u) {
    rgb8 plte_unit;
    csc::read_var_from_vector(plte_unit, pos, blob.data);
    s.full_palette.emplace_back(std::move(plte_unit));
  }

  s.crc_adler = blob.crc_adler;
  return csc::section_code_t::success;
}

csc::section_code_t consume_chunk(
    csc::IDAT& s,
    const csc::chunk& blob,
    const csc::IHDR& header,
    csc::inflater& infstream,
    cstd::vector<uint8_t>& generic_image) noexcept {
  try {
    const float pixel_size = (header.bit_depth / 8.f);
    const uint32_t channels = pixel_size_from_color_type(header.color_type);
    const uint32_t real_size = header.width * header.height * pixel_size * channels * 1.005f;
    generic_image.reserve(real_size); // после первого резервирования ничего не делает
    infstream.set_compressed_buffer(blob.data);
    do {
      infstream.inflate();
      auto range = infstream.value();
      std::ranges::copy(range, std::back_inserter(generic_image));
    } while (!infstream.done());
  } catch (const cstd::runtime_error& e) {
#ifndef NDEBUG
    using cstd::operator<<;
    cstd::cerr << "Недесериализован чанк IDAT, причина ошибки: " << e.what() << '\n';
#endif
  }
  s.crc_adler = blob.crc_adler;
  return csc::section_code_t::success;
}

csc::section_code_t consume_chunk(csc::IEND& s, const csc::chunk& blob) noexcept {
  s.crc_adler = blob.crc_adler;
  return csc::section_code_t::success;
}

} // namespace csc
