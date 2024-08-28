module;

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <ranges>

export module csc.png.picture;
export import :signature;

export import csc.png.picture.sections;

#ifndef NDEBUG
import cstd.stl_wrap.ios;
import cstd.stl_wrap.iostream;
#endif
import cstd.stl_wrap.variant;
import cstd.stl_wrap.vector;

export namespace csc {

class picture {
 public:
  csc::png_signature m_start;
  csc::v_sections m_structured{};
  cstd::vector<uint8_t> m_image_data{};
  csc::IHDR m_header;
  csc::IEND m_eof_block;

 public:
  picture() = default;

  const csc::png_signature& start() const {
    return m_start;
  }
  const csc::IHDR& header() const {
    return m_header;
  }
  const csc::IEND& eof_block() const {
    return m_eof_block;
  }

  csc::png_signature& start() {
    return m_start;
  }
  csc::IHDR& header() {
    return m_header;
  }
  csc::IEND& eof_block() {
    return m_eof_block;
  }
};

#ifndef NDEBUG
struct time_info_localed {
  char buffer[80];
  time_info_localed() = delete;
  time_info_localed(const std::tm& datatime) noexcept {
    std::strftime(buffer, 80, "%A, %d %b %Y %T", &datatime);
  }
};
cstd::ostream& operator<<(cstd::ostream& os, const csc::picture& image) {
  using cstd::operator<<;
  using ct = e_color_type;
  const csc::IHDR& h = cstd::get<csc::IHDR>(image.m_structured[0]);
  const auto is_plte_type = [](const v_section& sn) { return cstd::holds_alternative<csc::PLTE>(sn); };
  const auto is_bkgd_type = [](const v_section& sn) { return cstd::holds_alternative<csc::bKGD>(sn); };
  const auto is_time_type = [](const v_section& sn) { return cstd::holds_alternative<csc::tIME>(sn); };
  const auto is_chrm_type = [](const v_section& sn) { return cstd::holds_alternative<csc::cHRM>(sn); };
  const auto is_gama_type = [](const v_section& sn) { return cstd::holds_alternative<csc::gAMA>(sn); };
  const auto is_hist_type = [](const v_section& sn) { return cstd::holds_alternative<csc::hIST>(sn); };
  const auto is_phys_type = [](const v_section& sn) { return cstd::holds_alternative<csc::pHYs>(sn); };
  const auto plte_pos = std::ranges::find_if(image.m_structured, is_plte_type);
  const auto bkgd_pos = std::ranges::find_if(image.m_structured, is_bkgd_type);
  const auto time_pos = std::ranges::find_if(image.m_structured, is_time_type);
  const auto chrm_pos = std::ranges::find_if(image.m_structured, is_chrm_type);
  const auto gama_pos = std::ranges::find_if(image.m_structured, is_gama_type);
  const auto hist_pos = std::ranges::find_if(image.m_structured, is_hist_type);
  const auto phys_pos = std::ranges::find_if(image.m_structured, is_phys_type);
  const char* color_type = (h.color_type == ct::bw) ? "чёрно-белый"
      : (h.color_type == ct::rgb)                   ? "цветной"
      : (h.color_type == ct::indexed)               ? "индексное изображение"
      : (h.color_type == ct::bwa)                   ? "чёрно-белый (альфа-канал)"
      : (h.color_type == ct::rgba)                  ? "цветной (альфа-канал)"
                                                    : "unknown";
  const char* interlace = (h.interlace == e_interlace::none) ? "Отсутствует"
      : (h.interlace == e_interlace::adam7)                  ? "Есть (Adam7)"
                                                             : "unknown";

  for (uint8_t byte : image.start().data) {
    os << cstd::hex << +byte << ' ';
  }
  os << cstd::dec << "\nОсновные данные:\n";
  os << "Длина: " << cstd::dec << h.width << " Ширина: " << h.height << '\n';
  os << "Битовая глубина: " << +h.bit_depth << '\n';
  os << "Цветность: " << color_type << '\n';

  os << "Метод компрессии: " << ((h.compression == e_compression::deflate) ? "Алгоритм deflate" : "unknown") << '\n';
  os << "Фильтрация: " << ((h.filter == e_filter::adaptive) ? "Стандарт (adaptive)" : "unknown") << '\n';
  os << "Поддержка межстрочности: " << interlace << '\n';
  const float image_size_mb = image.m_image_data.size() / 1024.f / 1024.f;
  os << "Размер изображения в памяти: " << image_size_mb << " Мб.\n";
  os << "Размер изображения, б: " << image.m_image_data.size() << '\n';
  os << "Ёмкость изображения, б: " << image.m_image_data.capacity() << '\n';

  if (plte_pos != image.m_structured.cend()) {
    os << "Палитра (в байтах): \n";
    const csc::PLTE& plte = cstd::get<csc::PLTE>(*plte_pos);
    const auto& p = plte.full_palette;
    for (auto unit_it = p.begin(), last_row_it = unit_it - 1; unit_it != p.end(); ++unit_it) {
      os << cstd::hex << +unit_it->r << ' ' << +unit_it->g << ' ' << +unit_it->b << "  ";
      if (std::distance(last_row_it, unit_it) == h.bit_depth)
        os << '\n', last_row_it = unit_it;
    }
    os << '\n' << cstd::dec << "Размер палитры: " << plte.full_palette.size() << '\n';
  } else {
    os << "Палитра отсутствует.\n";
  }

  if (bkgd_pos != image.m_structured.cend()) {
    const auto max_pixel_value = static_cast<uint16_t>((1ul << h.bit_depth) - 1);
    os << "Заливка прозрачного фона: \n";
    const csc::bKGD& bkgd = cstd::get<csc::bKGD>(*bkgd_pos);
    if (bkgd.color_type == csc::e_pixel_view_id::rgb8) {
      const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::rgb8);
      const auto& pixel = cstd::get<variant_idx>(bkgd.color);
      os << "Красный: " << +pixel.r << " Зелёный: " << +pixel.g << " Синий: " << +pixel.b
         << " (Max: " << max_pixel_value << ")\n";
    } else if (bkgd.color_type == csc::e_pixel_view_id::rgb16) {
      const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::rgb16);
      const auto& pixel = cstd::get<variant_idx>(bkgd.color);
      os << "Красный: " << +pixel.r << " Зелёный: " << +pixel.g << " Синий: " << +pixel.b
         << " (Max: " << max_pixel_value << ")\n";
    } else if (bkgd.color_type == csc::e_pixel_view_id::bw8) {
      const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::bw8);
      const auto& pixel = cstd::get<variant_idx>(bkgd.color);
      os << "Яркость: " << +pixel.bw << " (Max: " << max_pixel_value << ")\n";
    } else if (bkgd.color_type == csc::e_pixel_view_id::bw16) {
      const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::bw16);
      const auto& pixel = cstd::get<variant_idx>(bkgd.color);
      os << "Яркость: " << pixel.bw << " (Max: " << max_pixel_value << ")\n";
    } else if (bkgd.color_type == csc::e_pixel_view_id::plte_index) {
      const auto variant_idx = static_cast<uint8_t>(csc::e_pixel_view_id::plte_index);
      const auto& pixel = cstd::get<variant_idx>(bkgd.color);
      os << "Значение индекса: " << +pixel.idx << " (Max: " << max_pixel_value << ")\n";
    }
  } else {
    os << "Заливка отсутствует. По умолчанию черный.\n";
  }

  if (time_pos != image.m_structured.cend()) {
    os << "Дата-время изменения файла: \n";
    const csc::tIME& time = cstd::get<csc::tIME>(*time_pos);
    auto time_copy = time.time_data;
    const time_info_localed display(time_copy);
    os << display.buffer << '\n';
  } else {
    os << "Информация о времени сохранения файла отсутствует.\n";
  }

  if (chrm_pos != image.m_structured.cend()) {
    const csc::cHRM& chrm = cstd::get<csc::cHRM>(*chrm_pos);
    os << "Значения основных цветов и белой точки:\n";
    os << "Белый - [ " << chrm.white_x / 100'000.f << " ; " << chrm.white_y / 100'000.f << " ]" << '\n';
    os << "Красный - [ " << chrm.red_x / 100'000.f << " ; " << chrm.red_y / 100'000.f << " ]" << '\n';
    os << "Зелёный - [ " << chrm.green_x / 100'000.f << " ; " << chrm.green_y / 100'000.f << " ]" << '\n';
    os << "Синий - [ " << chrm.blue_x / 100'000.f << " ; " << chrm.blue_y / 100'000.f << " ]" << '\n';
  } else {
    os << "Данные о значениях цветового координатного пространства отсутствуют.\n";
  }

  if (gama_pos != image.m_structured.cend()) {
    const csc::gAMA& gama = cstd::get<csc::gAMA>(*gama_pos);
    os << "Значения гаммы:\n";
    os << "Гамма: " << gama.gamma / 100'000.f << '\n';
  } else {
    os << "Значения гаммы для текущего изображения отсутствуют.\n";
  }
  if (hist_pos != image.m_structured.cend()) {
    const csc::hIST& hist = cstd::get<csc::hIST>(*hist_pos);
    os << "Гистограмма:\n";
    for (const auto& frequency : hist.histogram) {
      os << frequency << ' ';
    }
    os << "\n Размер гистограммы: " << hist.histogram.size() << '\n';
  } else {
    os << "Гистограмма для изображения отсутствует.\n";
  }

  if (phys_pos != image.m_structured.cend()) {
    const csc::pHYs& phys = cstd::get<csc::pHYs>(*phys_pos);
    os << "Фактический размер изображения в пикселях:\n";
    const char* specifier = (phys.unit_type == csc::unit_specifier::metric) ? "метр" : "единицy";
    os << phys.pixels_per_unit_x << " пикселей на " << specifier << " по оси X\n";
    os << phys.pixels_per_unit_y << " пикселей на " << specifier << " по оси Y\n";
  } else {
    os << "Данные о фактическом разрешении отсутствуют.\n";
  }
  if (gama_pos != image.m_structured.cend()) {
    const csc::gAMA& gama = cstd::get<csc::gAMA>(*gama_pos);
    os << "Значения гаммы:\n";
    os << "Гамма: " << gama.gamma / 100'000.f << '\n';
  } else {
    os << "Значения гаммы для текущего изображения отсутствуют.\n";
  }

  return os;
}
#endif
} // namespace csc
