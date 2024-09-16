module;

#include <bits/stl_iterator.h>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <ctime>

module csc.png.picture_debug.write_section_to_ostream:overloads;

import csc.png.picture;

import stl.ios;
import stl.ostream;

namespace csc {
constexpr const char* bring_color_type(csc::e_color_type t) {
  using enum csc::e_color_type;
  switch (t) {
    case bw:
      return "чёрно-белый";
    case rgb:
      return "цветной";
    case indexed:
      return "индексное изображение";
    case rgba:
      return "цветной (альфа-канал)";
    case bwa:
      return "чёрно-белый (альфа-канал)";
    default:
      return "Не распознаётся.";
  }
}
constexpr const char* bring_compression(csc::e_compression t) {
  using enum csc::e_compression;
  switch (t) {
    case deflate:
      return "Алгоритм deflate";
    default:
      return "Не распознаётся.";
  }
}
constexpr const char* bring_filter(csc::e_filter t) {
  using enum csc::e_filter;
  switch (t) {
    case adaptive:
      return "Стандарт (adaptive)";
    default:
      return "Не распознаётся.";
  }
}
constexpr const char* bring_interlace(csc::e_interlace t) {
  using enum csc::e_interlace;
  switch (t) {
    case adam7:
      return "Есть (Adam7)";
    case none:
      return "Отсутствует";
    default:
      return "Не распознаётся.";
  }
}
struct time_info_format {
  char buffer[80];
  time_info_format() = delete;
  time_info_format(const std::tm& datatime) noexcept {
    std::strftime(buffer, 80, "%A, %d %b %Y %T", &datatime);
  }
};

void write_section_to_ostream(const csc::IHDR& ihdr, std::ostream& os) {
  using std::operator<<;
  os << "Длина: " << std::dec << ihdr.width << " Ширина: " << ihdr.height << '\n';
  os << "Битовая глубина: " << +ihdr.bit_depth << '\n';
  os << "Цветность: " << csc::bring_color_type(ihdr.color_type) << '\n';

  os << "Метод компрессии: " << csc::bring_compression(ihdr.compression) << '\n';
  os << "Фильтрация: " << csc::bring_filter(ihdr.filter) << '\n';
  os << "Поддержка межстрочности: " << csc::bring_interlace(ihdr.interlace) << '\n';
}

void write_section_to_ostream(const csc::PLTE& plte, std::ostream& os, uint8_t bit_depth) {
  using std::operator<<;
  os << "Данные палитры: \n";
  const auto& p = plte.full_palette;
  for (auto unit_it = p.begin(), last_row_it = unit_it - 1; unit_it != p.end(); ++unit_it) {
    os << std::hex << +unit_it->r << ' ' << +unit_it->g << ' ' << +unit_it->b << "  ";
    if (std::distance(last_row_it, unit_it) == bit_depth)
      os << '\n', last_row_it = unit_it;
  }
  os << '\n' << std::dec << "Размер палитры: " << plte.full_palette.size() << '\n';
}

void write_section_to_ostream(const csc::IEND&, std::ostream&) {
}

void write_section_to_ostream(const csc::bKGD& bkgd, std::ostream& os, uint8_t bit_depth) {
  using std::operator<<;
  os << "Заливка прозрачного фона: \n";
  const auto max_pixel_value = static_cast<uint16_t>((1ul << bit_depth) - 1);
  if (bkgd.color_type == csc::e_pixel_view_id::rgb8) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::rgb8);
    const auto& pixel = std::get<variant_idx>(bkgd.color);
    os << "Красный: " << +pixel.r << " Зелёный: " << +pixel.g << " Синий: " << +pixel.b << " (Max: " << max_pixel_value
       << ")\n";
  } else if (bkgd.color_type == csc::e_pixel_view_id::rgb16) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::rgb16);
    const auto& pixel = std::get<variant_idx>(bkgd.color);
    os << "Красный: " << +pixel.r << " Зелёный: " << +pixel.g << " Синий: " << +pixel.b << " (Max: " << max_pixel_value
       << ")\n";
  } else if (bkgd.color_type == csc::e_pixel_view_id::bw8) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::bw8);
    const auto& pixel = std::get<variant_idx>(bkgd.color);
    os << "Яркость: " << +pixel.bw << " (Max: " << max_pixel_value << ")\n";
  } else if (bkgd.color_type == csc::e_pixel_view_id::bw16) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::bw16);
    const auto& pixel = std::get<variant_idx>(bkgd.color);
    os << "Яркость: " << pixel.bw << " (Max: " << max_pixel_value << ")\n";
  } else if (bkgd.color_type == csc::e_pixel_view_id::plte_index) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::plte_index);
    const auto& pixel = std::get<variant_idx>(bkgd.color);
    os << "Значение индекса: " << +pixel.idx << " (Max: " << max_pixel_value << ")\n";
  }
}

void write_section_to_ostream(const csc::cHRM& chrm, std::ostream& os) {
  using std::operator<<;
  os << "Значения основных цветов и белой точки:\n";
  os << "Белый - [ " << chrm.white_x / 100'000.f << " ; " << chrm.white_y / 100'000.f << " ]" << '\n';
  os << "Красный - [ " << chrm.red_x / 100'000.f << " ; " << chrm.red_y / 100'000.f << " ]" << '\n';
  os << "Зелёный - [ " << chrm.green_x / 100'000.f << " ; " << chrm.green_y / 100'000.f << " ]" << '\n';
  os << "Синий - [ " << chrm.blue_x / 100'000.f << " ; " << chrm.blue_y / 100'000.f << " ]" << '\n';
}
void write_section_to_ostream(const csc::tIME& time, std::ostream& os) {
  using std::operator<<;
  os << "Дата-время изменения файла: \n";
  auto time_copy = time.time_data;
  const time_info_format display(time_copy);
  os << display.buffer << '\n';
}
void write_section_to_ostream(const csc::gAMA& gama, std::ostream& os) {
  using std::operator<<;
  os << "Значения гаммы:\n";
  os << "Гамма: " << gama.gamma / 100'000.f << '\n';
}
void write_section_to_ostream(const csc::hIST& hist, std::ostream& os) {
  using std::operator<<;
  os << "Гистограмма:\n";
  for (const auto& frequency : hist.histogram) {
    os << frequency << ' ';
  }
  os << "\n Размер гистограммы: " << hist.histogram.size() << '\n';
}
void write_section_to_ostream(const csc::pHYs& phys, std::ostream& os) {
  using std::operator<<;
  os << "Фактический размер изображения в пикселях:\n";
  const char* specifier = (phys.unit_type == csc::unit_specifier::metric) ? "метр" : "единицy";
  os << phys.pixels_per_unit_x << " пикселей на " << specifier << " по оси X\n";
  os << phys.pixels_per_unit_y << " пикселей на " << specifier << " по оси Y\n";
}
void write_section_to_ostream(const csc::tRNS& trns, std::ostream& os, uint8_t bit_depth) {
  using std::operator<<;
  os << "Значение(-я) псевдо-прозрачности:\n";
  const auto max_pixel_value = static_cast<uint16_t>((1ul << bit_depth) - 1);
  if (trns.color_type == csc::e_pixel_view_trns_id::rgb8) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_trns_id::rgb8);
    const auto& pixel = std::get<variant_idx>(trns.color);
    os << "Красный: " << +pixel.r << " Зелёный: " << +pixel.g << " Синий: " << +pixel.b << " (Max: " << max_pixel_value
       << ")\n";
  } else if (trns.color_type == csc::e_pixel_view_trns_id::rgb16) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_trns_id::rgb16);
    const auto& pixel = std::get<variant_idx>(trns.color);
    os << "Красный: " << +pixel.r << " Зелёный: " << +pixel.g << " Синий: " << +pixel.b << " (Max: " << max_pixel_value
       << ")\n";
  } else if (trns.color_type == csc::e_pixel_view_trns_id::bw8) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_trns_id::bw8);
    const auto& pixel = std::get<variant_idx>(trns.color);
    os << "Яркость: " << +pixel.bw << " (Max: " << max_pixel_value << ")\n";
  } else if (trns.color_type == csc::e_pixel_view_trns_id::bw16) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_trns_id::bw16);
    const auto& pixel = std::get<variant_idx>(trns.color);
    os << "Яркость: " << pixel.bw << " (Max: " << max_pixel_value << ")\n";
  } else if (trns.color_type == csc::e_pixel_view_trns_id::plte_indices) {
    constexpr const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_trns_id::plte_indices);
    const auto& pixels = std::get<variant_idx>(trns.color);
    for (auto pixel : pixels) {
      os << std::hex << +pixel.idx << ' ';
    }
    os << std::dec << "(Max: " << max_pixel_value << ")\n";
  }
}

} // namespace csc
