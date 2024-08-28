module;
#include <algorithm>
#include <cstdint>
#include <ctime>
#include <ranges>
module csc.png.deserializer.consume_chunk:overloads;

import cstd.stl_wrap.vector;
import cstd.stl_wrap.stdexcept;
export import cstd.stl_wrap.variant;

#ifndef NDEBUG
import cstd.stl_wrap.iostream;
#endif

export import csc.png.deserializer.consume_chunk.inflater;
export import csc.png.deserializer.consume_chunk.buf_reader;
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
  csc::buf_reader rdr(blob.data.data());
  using enum csc::e_color_type;
  const auto& type = header.color_type;
  if ((type == rgb || type == rgba) && header.bit_depth == 8) {
    const auto r16 = rdr.read<uint16_t>(), g16 = rdr.read<uint16_t>(), b16 = rdr.read<uint16_t>();
    s.color = csc::rgb8{static_cast<uint8_t>(r16), static_cast<uint8_t>(g16), static_cast<uint8_t>(b16)};
    s.color_type = csc::e_pixel_view_id::rgb8;
  } else if ((type == rgb || type == rgba) && header.bit_depth == 16) {
    const auto r16 = rdr.read<uint16_t>(), g16 = rdr.read<uint16_t>(), b16 = rdr.read<uint16_t>();
    s.color = csc::rgb16{r16, g16, b16}, s.color_type = csc::e_pixel_view_id::rgb16;
  } else if ((type == bw || type == bwa) && header.bit_depth <= 8) {
    const auto bw16 = rdr.read<uint16_t>();
    s.color = csc::bw8{static_cast<uint8_t>(bw16)}, s.color_type = csc::e_pixel_view_id::bw8;
  } else if ((type == bw || type == bwa) && header.bit_depth == 16) {
    const auto bw16 = rdr.read<uint16_t>();
    s.color = csc::bw16{bw16}, s.color_type = csc::e_pixel_view_id::bw16;
  } else if (type == indexed) {
    const auto idx8 = rdr.read<uint8_t>();
    s.color = csc::plte_index{idx8}, s.color_type = csc::e_pixel_view_id::plte_index;
  }
  return csc::section_code_t::success;
}
int8_t bring_utc_offset() noexcept {
  std::time_t current_time;
  std::time(&current_time);
  const auto p_localtime = std::localtime(&current_time);
  return static_cast<int8_t>((p_localtime) ? (p_localtime->tm_gmtoff / 60 / 60) : 0); // секунды в часы
}
csc::section_code_t consume_chunk(csc::tIME& s, const csc::chunk& blob) noexcept {
  csc::buf_reader rdr(blob.data.data());
  const auto year = rdr.read<uint16_t>(); // в спецификации год представлен как uint16, а в ctime как int
  const auto mon = rdr.read<uint8_t>(), mday = rdr.read<uint8_t>();
  const auto hour = rdr.read<uint8_t>(), min = rdr.read<uint8_t>(), sec = rdr.read<uint8_t>();
  auto& t = s.time_data;
  t.tm_wday = 0, t.tm_yday = 0, t.tm_isdst = -1; // летнее время (dst) не опознано
  const auto t_mon = mon - 1; // png спецификация ведет отсчёт месяцев с 1, ctime - с нуля
  const auto t_year = year - 1900; // ctime - ведет отсчет годов с 1900 года
  const auto t_hour = hour + csc::bring_utc_offset(); // задаём час, учитывая наше локальное время
  t.tm_year = static_cast<int>(t_year), t.tm_mon = static_cast<int>(t_mon), t.tm_mday = static_cast<int>(mday);
  t.tm_hour = static_cast<int>(t_hour), t.tm_min = static_cast<int>(min), t.tm_sec = static_cast<int>(sec);
  return csc::section_code_t::success;
}
} // namespace csc

namespace csc {

csc::section_code_t consume_chunk(csc::IHDR& s, const csc::chunk& blob) noexcept {
  csc::buf_reader rdr(blob.data.data());
  const auto width = rdr.read<uint32_t>(), height = rdr.read<uint32_t>();
  const auto bit_depth = rdr.read<uint8_t>(), color_type = rdr.read<uint8_t>();
  const auto compression = rdr.read<uint8_t>(), filter = rdr.read<uint8_t>(), interlace = rdr.read<uint8_t>();
  s.width = width, s.height = height, s.bit_depth = bit_depth, s.color_type = static_cast<e_color_type>(color_type);
  s.compression = static_cast<e_compression>(compression), s.filter = static_cast<e_filter>(filter);
  s.interlace = static_cast<e_interlace>(interlace);
  return csc::section_code_t::success;
}

csc::section_code_t consume_chunk(csc::PLTE& s, const csc::chunk& blob, const csc::IHDR& header) noexcept {
  if (blob.data.size() % 3 != 0 || blob.data.size() > (256 * 3))
    return csc::section_code_t::error;
  if (header.color_type == e_color_type::indexed && blob.data.size() / 3 > (1 << header.bit_depth))
    return csc::section_code_t::error;

  s.full_palette.reserve(256u);
  csc::buf_reader rdr(blob.data.data());
  for (uint32_t pos = 0u; pos < blob.data.size(); pos += 3u) {
    const auto idxr = rdr.read<uint8_t>(), idxg = rdr.read<uint8_t>(), idxb = rdr.read<uint8_t>();
    s.full_palette.emplace_back(csc::rgb8(idxr, idxg, idxb));
  }
  return csc::section_code_t::success;
}

csc::section_code_t consume_chunk(
    csc::IDAT&,
    const csc::chunk& blob,
    const csc::IHDR& header,
    csc::inflater& infstream,
    cstd::vector<uint8_t>& generic_image) noexcept {
  try {
    const float pixel_size = (header.bit_depth / 8.f);
    const uint32_t channels = pixel_size_from_color_type(header.color_type);
    const uint32_t real_size = header.width * header.height * pixel_size * channels * 1.015f;
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
  return csc::section_code_t::success;
}

csc::section_code_t consume_chunk(csc::IEND&, const csc::chunk&) noexcept {
  return csc::section_code_t::success;
}

} // namespace csc
