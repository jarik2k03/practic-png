module;

#include <algorithm>
#include <cstdint>
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
cstd::ostream& operator<<(cstd::ostream& os, const csc::picture& image) {
  using cstd::operator<<;
  using ct = e_color_type;
  const csc::IHDR& h = cstd::get<csc::IHDR>(image.m_structured[0]);
  const auto is_plte_type = [](const v_section& sn) { return cstd::holds_alternative<csc::PLTE>(sn); };
  const auto plte_pos = std::ranges::find_if(image.m_structured, is_plte_type);
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

  if (plte_pos != image.m_structured.end()) {
    os << "Палитра (в байтах): \n";
    const csc::PLTE& plte = cstd::get<csc::PLTE>(*plte_pos);
    const auto& p = plte.full_palette;
    for (auto unit_it = p.begin(), last_row_it = unit_it - 1; unit_it != p.end(); ++unit_it) {
      os << cstd::hex << +unit_it->r << ' ' << +unit_it->g << ' ' << +unit_it->b << "  ";
      if (std::distance(last_row_it, unit_it) == h.bit_depth)
        os << '\n', last_row_it = unit_it;
    }
    os << '\n' << cstd::dec << "Размер палитры: " << plte.full_palette.size() << '\n';
  }
  return os;
}
#endif
} // namespace csc
