
module;
#include <cstdint>
#include <ctime>

module csc.png.serializer.produce_chunk:utility;

import csc.png.picture.sections;

import stl.array;
import stl.variant;

namespace csc {
namespace png {

constexpr uint32_t calc_size_for_chunk(const png::IHDR& s) noexcept {
  return sizeof(s.width) + sizeof(s.height) + sizeof(s.bit_depth) + sizeof(s.color_type) + sizeof(s.compression) +
      sizeof(s.filter) + sizeof(s.interlace);
}
constexpr uint32_t calc_size_for_chunk(const png::PLTE& s) noexcept {
  return s.full_palette.size() * sizeof(decltype(s.full_palette[0]));
}
constexpr uint32_t calc_size_for_chunk(const png::IEND&) noexcept {
  return 0u;
}
constexpr uint32_t calc_size_for_chunk(const png::tIME&) noexcept {
  // size == year + mon + day + hour + minute + second
  return sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t);
}

constexpr uint32_t calc_size_for_chunk(const png::hIST& s) noexcept {
  return s.histogram.size() * sizeof(decltype(s.histogram[0]));
}
constexpr uint32_t calc_size_for_chunk(const png::pHYs& s) noexcept {
  return sizeof(s.pixels_per_unit_x) + sizeof(s.pixels_per_unit_y) + sizeof(s.unit_type);
}
constexpr uint32_t calc_size_for_chunk(const png::gAMA& s) noexcept {
  return sizeof(s.gamma);
}
constexpr uint32_t calc_size_for_chunk(const png::bKGD& s) noexcept {
  using enum png::e_pixel_view_id;
  switch (s.color_type) {
    case rgb8:
      return sizeof(png::rgb16); // rgb в чанке хранится с запасом - 2 байта на цвет
    case rgb16:
      return sizeof(png::rgb16);
    case bw8:
      return sizeof(png::bw16); // bw в чанке хранится с запасом - 2 байта на яркость
    case bw16:
      return sizeof(png::bw16);
    case plte_index:
      return sizeof(png::plte_index); // достаточно индекса палитры
    default:
      return 0u; // так принято.
  }
}
constexpr uint32_t calc_size_for_chunk(const png::cHRM& s) noexcept {
  return sizeof(s.white_x) + sizeof(s.white_y) + sizeof(s.red_x) + sizeof(s.red_y) + sizeof(s.green_x) +
      sizeof(s.green_y) + sizeof(s.blue_x) + sizeof(s.blue_y);
}
constexpr uint32_t calc_size_for_chunk(const png::tRNS& s) noexcept {
  using enum png::e_pixel_view_trns_id;
  const auto* const indices = std::get_if<static_cast<uint8_t>(plte_indices)>(&s.color);
  switch (s.color_type) {
    case rgb8:
      return sizeof(png::rgb16); // rgb в чанке хранится с запасом - 2 байта на цвет
    case rgb16:
      return sizeof(png::rgb16);
    case bw8:
      return sizeof(png::bw16); // bw в чанке хранится с запасом - 2 байта на яркость
    case bw16:
      return sizeof(png::bw16);
    case plte_indices:
      return (indices != nullptr) ? sizeof(png::plte_index) * indices->size() : 0u;
    default:
      return 0u; // так принято.
  }
}

int8_t bring_utc_offset() noexcept {
  std::time_t current_time;
  std::time(&current_time);
  const auto p_localtime = std::localtime(&current_time);
  return static_cast<int8_t>((p_localtime) ? (p_localtime->tm_gmtoff / 60 / 60) : 0); // секунды в часы
}

} // namespace png
} // namespace csc
