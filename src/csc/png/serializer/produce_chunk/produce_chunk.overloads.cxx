module;
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <ctime>
#include <zlib.h>
module csc.png.serializer.produce_chunk:overloads;

import :utility;

// export import csc.png.serializer.produce_chunk.inflater;
export import csc.png.serializer.produce_chunk.buf_writer;
export import csc.png.picture.sections;
export import csc.png.commons.chunk;

namespace csc {

auto create_crc32 = [](const cstd::array<char, 4>& name, const csc::u8unique_buffer& data) -> uint32_t {
  uint32_t crc = crc32(0ul, reinterpret_cast<const uint8_t*>(name.cbegin()), name.size());
  if (data.size != 0u)
    crc = crc32(crc, data.begin(), data.size);
  return crc;
};

csc::e_section_code produce_chunk(const csc::gAMA& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'g', 'A', 'M', 'A'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  wrt.write(s.gamma);

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::pHYs& s, csc::chunk& blob) noexcept {
  blob.chunk_name = cstd::array<char, 4>{'p', 'H', 'Y', 's'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  wrt.write(s.pixels_per_unit_x), wrt.write(s.pixels_per_unit_y);
  wrt.write(static_cast<uint8_t>(s.unit_type));

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::hIST& s, csc::chunk& blob) noexcept {
  blob.chunk_name = cstd::array<char, 4>{'h', 'I', 'S', 'T'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  auto write_index = [&wrt](auto idx) { wrt.write(idx); };
  std::ranges::for_each(s.histogram, write_index);

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::cHRM& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'c', 'H', 'R', 'M'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  wrt.write(s.white_x), wrt.write(s.white_y);
  wrt.write(s.red_x), wrt.write(s.red_y);
  wrt.write(s.green_x), wrt.write(s.green_y);
  wrt.write(s.blue_x), wrt.write(s.blue_y);

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::bKGD& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'b', 'K', 'G', 'D'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  if (cstd::holds_alternative<csc::rgb8>(s.color)) {
    const auto& color = cstd::get<csc::rgb8>(s.color);
    wrt.write(static_cast<uint16_t>(color.r)); // rgb8 хранится в файле как rgb16
    wrt.write(static_cast<uint16_t>(color.g)); // rgb8 хранится в файле как rgb16
    wrt.write(static_cast<uint16_t>(color.b)); // rgb8 хранится в файле как rgb16
  } else if (cstd::holds_alternative<csc::rgb16>(s.color)) {
    const auto& color = cstd::get<csc::rgb16>(s.color);
    wrt.write(color.r), wrt.write(color.g),
        wrt.write(color.b); // rgb16 записываем как есть
  } else if (cstd::holds_alternative<csc::bw8>(s.color)) {
    const auto& color = cstd::get<csc::bw8>(s.color);
    wrt.write(static_cast<uint16_t>(color.bw)); // bw8 хранится в файле как bw16
  } else if (cstd::holds_alternative<csc::bw16>(s.color)) {
    const auto& color = cstd::get<csc::bw16>(s.color);
    wrt.write(color.bw); // bw16 записываем как есть
  } else if (cstd::holds_alternative<csc::plte_index>(s.color)) {
    const auto& color = cstd::get<csc::plte_index>(s.color);
    wrt.write(color.idx); // записываем индекс
  }

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::tIME& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'t', 'I', 'M', 'E'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  wrt.write(static_cast<uint16_t>(s.time_data.tm_year + 1900)); // обратно в простой формат
  wrt.write(static_cast<uint8_t>(s.time_data.tm_mon + 1)); // месяцы в пнг начинаются с единицы
  wrt.write(static_cast<uint8_t>(s.time_data.tm_mday));
  wrt.write(static_cast<uint8_t>(s.time_data.tm_hour - csc::bring_utc_offset())); // время по гринвичу
  wrt.write(static_cast<uint8_t>(s.time_data.tm_min));
  wrt.write(static_cast<uint8_t>(s.time_data.tm_sec));

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::IHDR& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'I', 'H', 'D', 'R'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  wrt.write(s.width), wrt.write(s.height), wrt.write(s.bit_depth);
  wrt.write(static_cast<uint8_t>(s.color_type)), wrt.write(static_cast<uint8_t>(s.compression));
  wrt.write(static_cast<uint8_t>(s.filter)), wrt.write(static_cast<uint8_t>(s.interlace));

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::PLTE& s, csc::chunk& blob) noexcept {
  blob.crc_adler = std::rand(); // не реализовано вычисление контрольной суммы
  blob.chunk_name = cstd::array<char, 4>{'P', 'L', 'T', 'E'};
  const auto chunk_size = csc::calc_size_for_chunk(s);
  blob.buffer = csc::make_unique_buffer<uint8_t>(chunk_size);

  csc::buf_writer wrt(blob.buffer.data.get());
  auto write_plte_unit = [&wrt](const auto& unit) { wrt.write(unit.r), wrt.write(unit.g), wrt.write(unit.b); };
  std::ranges::for_each(s.full_palette, write_plte_unit);

  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

csc::e_section_code produce_chunk(const csc::IEND&, csc::chunk& blob) noexcept {
  blob.chunk_name = cstd::array<char, 4>{'I', 'E', 'N', 'D'};
  blob.crc_adler = create_crc32(blob.chunk_name, blob.buffer);
  return csc::e_section_code::success;
}

} // namespace csc
