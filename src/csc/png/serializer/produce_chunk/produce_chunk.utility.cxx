
module;
#include <cstdint>
#include <ctime>
module csc.png.serializer.produce_chunk:utility;
import csc.png.picture.sections;

namespace csc {

constexpr uint32_t calc_size_for_chunk(const csc::IHDR& s) noexcept {
  return sizeof(s.width) + sizeof(s.height) + sizeof(s.bit_depth) + sizeof(s.color_type) + sizeof(s.compression) +
      sizeof(s.filter) + sizeof(s.interlace);
}
constexpr uint32_t calc_size_for_chunk(const csc::PLTE& s) noexcept {
  return s.full_palette.size() * sizeof(decltype(s.full_palette[0]));
}
constexpr uint32_t calc_size_for_chunk(const csc::IEND&) noexcept {
  return 0u;
}
constexpr uint32_t calc_size_for_chunk(const csc::tIME&) noexcept {
  // size == year + mon + day + hour + minute + second
  return sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t);
}

constexpr uint32_t calc_size_for_chunk(const csc::hIST& s) noexcept {
  return s.histogram.size() * sizeof(decltype(s.histogram[0]));
}
constexpr uint32_t calc_size_for_chunk(const csc::pHYs& s) noexcept {
  return sizeof(s.pixels_per_unit_x) + sizeof(s.pixels_per_unit_y) + sizeof(s.unit_type);
}
constexpr uint32_t calc_size_for_chunk(const csc::gAMA& s) noexcept {
  return sizeof(s.gamma);
}
constexpr uint32_t calc_size_for_chunk(const csc::bKGD& s) noexcept {
  using enum csc::e_pixel_view_id;
  switch (s.color_type) {
    case rgb8:
      return sizeof(csc::rgb16); // rgb в чанке хранится с запасом - 2 байта на цвет
    case rgb16:
      return sizeof(csc::rgb16);
    case bw8:
      return sizeof(csc::bw16); // bw в чанке хранится с запасом - 2 байта на яркость
    case bw16:
      return sizeof(csc::bw16);
    case plte_index:
      return sizeof(csc::plte_index); // достаточно индекса палитры
    default:
      return 0u; // так принято.
  }
}
constexpr uint32_t calc_size_for_chunk(const csc::cHRM& s) noexcept {
  return sizeof(s.white_x) + sizeof(s.white_y) + sizeof(s.red_x) + sizeof(s.red_y) + sizeof(s.green_x) +
      sizeof(s.green_y) + sizeof(s.blue_x) + sizeof(s.blue_y);
}

int8_t bring_utc_offset() noexcept {
  std::time_t current_time;
  std::time(&current_time);
  const auto p_localtime = std::localtime(&current_time);
  return static_cast<int8_t>((p_localtime) ? (p_localtime->tm_gmtoff / 60 / 60) : 0); // секунды в часы
}

} // namespace csc
