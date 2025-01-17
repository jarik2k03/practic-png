module;
#include <bits/move.h>
#include <cstdint>
#include <ctime>
module csc.png.deserializer.consume_chunk:overloads;

import :utility;

// import csc.png.deserializer.consume_chunk.inflater;
export import csc.png.deserializer.consume_chunk.buf_reader;
export import csc.png.picture.sections;
export import csc.png.commons.chunk;

namespace csc {
namespace png {

png::e_section_code consume_chunk(png::gAMA& s, const png::chunk& blob) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  const auto gamma = rdr.read<uint32_t>();
  s.gamma = gamma;
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::pHYs& s, const png::chunk& blob) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  const auto pixels_x = rdr.read<uint32_t>(), pixels_y = rdr.read<uint32_t>();
  const auto specifier = rdr.read<uint8_t>();
  s.pixels_per_unit_x = pixels_x, s.pixels_per_unit_y = pixels_y;
  s.unit_type = static_cast<png::unit_specifier>(specifier);
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::hIST& s, const png::chunk& blob) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  s.histogram.reserve(256ul); // под максимальный размер палитры
  for (uint32_t pos = 0u; pos < blob.buffer.size(); pos += 2u) {
    const auto freq = rdr.read<uint16_t>();
    s.histogram.emplace_back(freq);
  }
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::cHRM& s, const png::chunk& blob) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  const auto wh_x = rdr.read<uint32_t>(), wh_y = rdr.read<uint32_t>();
  const auto r_x = rdr.read<uint32_t>(), r_y = rdr.read<uint32_t>();
  const auto g_x = rdr.read<uint32_t>(), g_y = rdr.read<uint32_t>();
  const auto b_x = rdr.read<uint32_t>(), b_y = rdr.read<uint32_t>();
  s.white_x = wh_x, s.white_y = wh_y, s.red_x = r_x, s.red_y = r_y;
  s.green_x = g_x, s.green_y = g_y, s.blue_x = b_x, s.blue_y = b_y;
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::bKGD& s, const png::chunk& blob, const png::IHDR& header) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  using enum png::e_color_type;
  const auto& type = header.color_type;
  if ((type == rgb || type == rgba) && header.bit_depth == 8) {
    const auto r16 = rdr.read<uint16_t>(), g16 = rdr.read<uint16_t>(), b16 = rdr.read<uint16_t>();
    s.color = png::rgb8{static_cast<uint8_t>(r16), static_cast<uint8_t>(g16), static_cast<uint8_t>(b16)};
    s.color_type = png::e_pixel_view_id::rgb8;
  } else if ((type == rgb || type == rgba) && header.bit_depth == 16) {
    const auto r16 = rdr.read<uint16_t>(), g16 = rdr.read<uint16_t>(), b16 = rdr.read<uint16_t>();
    s.color = png::rgb16{r16, g16, b16}, s.color_type = png::e_pixel_view_id::rgb16;
  } else if ((type == bw || type == bwa) && header.bit_depth <= 8) {
    const auto bw16 = rdr.read<uint16_t>();
    s.color = png::bw8{static_cast<uint8_t>(bw16)}, s.color_type = png::e_pixel_view_id::bw8;
  } else if ((type == bw || type == bwa) && header.bit_depth == 16) {
    const auto bw16 = rdr.read<uint16_t>();
    s.color = png::bw16{bw16}, s.color_type = png::e_pixel_view_id::bw16;
  } else if (type == indexed) {
    const auto idx8 = rdr.read<uint8_t>();
    s.color = png::plte_index{idx8}, s.color_type = png::e_pixel_view_id::plte_index;
  }
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::tIME& s, const png::chunk& blob) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  const auto year = rdr.read<uint16_t>(); // в спецификации год представлен как
                                          // uint16, а в ctime как int
  const auto mon = rdr.read<uint8_t>(), mday = rdr.read<uint8_t>();
  const auto hour = rdr.read<uint8_t>(), min = rdr.read<uint8_t>(), sec = rdr.read<uint8_t>();
  auto& t = s.time_data;
  t.tm_wday = 0, t.tm_yday = 0,
  t.tm_isdst = -1; // летнее время (dst) не опознано
  const auto t_mon = mon - 1; // png спецификация ведет отсчёт месяцев с 1, ctime - с нуля
  const auto t_year = year - 1900; // ctime - ведет отсчет годов с 1900 года
  const auto t_hour = hour + png::bring_utc_offset(); // задаём час, учитывая наше локальное время
  t.tm_year = static_cast<int>(t_year), t.tm_mon = static_cast<int>(t_mon), t.tm_mday = static_cast<int>(mday);
  t.tm_hour = static_cast<int>(t_hour), t.tm_min = static_cast<int>(min), t.tm_sec = static_cast<int>(sec);
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::IHDR& s, const png::chunk& blob) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  const auto width = rdr.read<uint32_t>(), height = rdr.read<uint32_t>();
  const auto bit_depth = rdr.read<uint8_t>(), color_type = rdr.read<uint8_t>();
  const auto compression = rdr.read<uint8_t>(), filter = rdr.read<uint8_t>(), interlace = rdr.read<uint8_t>();
  s.width = width, s.height = height, s.bit_depth = bit_depth, s.color_type = static_cast<e_color_type>(color_type);
  s.compression = static_cast<e_compression>(compression), s.filter = static_cast<e_filter>(filter);
  s.interlace = static_cast<e_interlace>(interlace);
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::PLTE& s, const png::chunk& blob, const png::IHDR& header) noexcept {
  if (blob.buffer.size() % 3 != 0 || blob.buffer.size() > (256 * 3))
    return png::e_section_code::error;
  if (header.color_type == e_color_type::indexed && blob.buffer.size() / 3 > (1 << header.bit_depth))
    return png::e_section_code::error;

  s.full_palette.reserve(256u);
  png::buf_reader rdr(blob.buffer.data());
  for (uint32_t pos = 0u; pos < blob.buffer.size(); pos += 3u) {
    const auto idxr = rdr.read<uint8_t>(), idxg = rdr.read<uint8_t>(), idxb = rdr.read<uint8_t>();
    s.full_palette.emplace_back(png::rgb8(idxr, idxg, idxb));
  }
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::IEND&, const png::chunk&) noexcept {
  return png::e_section_code::success;
}

png::e_section_code consume_chunk(png::tRNS& s, const png::chunk& blob, const png::IHDR& header) noexcept {
  png::buf_reader rdr(blob.buffer.data());
  using enum png::e_color_type;
  const auto& type = header.color_type;
  if ((type == rgb || type == rgba) && header.bit_depth == 8) {
    const auto r16 = rdr.read<uint16_t>(), g16 = rdr.read<uint16_t>(), b16 = rdr.read<uint16_t>();
    s.color = png::rgb8{static_cast<uint8_t>(r16), static_cast<uint8_t>(g16), static_cast<uint8_t>(b16)};
    s.color_type = png::e_pixel_view_trns_id::rgb8;
  } else if ((type == rgb || type == rgba) && header.bit_depth == 16) {
    const auto r16 = rdr.read<uint16_t>(), g16 = rdr.read<uint16_t>(), b16 = rdr.read<uint16_t>();
    s.color = png::rgb16{r16, g16, b16}, s.color_type = png::e_pixel_view_trns_id::rgb16;
  } else if ((type == bw || type == bwa) && header.bit_depth <= 8) {
    const auto bw16 = rdr.read<uint16_t>();
    s.color = png::bw8{static_cast<uint8_t>(bw16)}, s.color_type = png::e_pixel_view_trns_id::bw8;
  } else if ((type == bw || type == bwa) && header.bit_depth == 16) {
    const auto bw16 = rdr.read<uint16_t>();
    s.color = png::bw16{bw16}, s.color_type = png::e_pixel_view_trns_id::bw16;
  } else if (type == indexed) {
    std::vector<png::plte_index> color_indices; // равносильно vector<uint8_t>
    color_indices.reserve(256u);
    for (const auto _ : blob.buffer) {
      const auto idx8 = rdr.read<uint8_t>();
      color_indices.emplace_back(png::plte_index{idx8});
    }
    s.color = std::move(color_indices), s.color_type = png::e_pixel_view_trns_id::plte_indices;
  }
  return png::e_section_code::success;
}
} // namespace png
} // namespace csc
