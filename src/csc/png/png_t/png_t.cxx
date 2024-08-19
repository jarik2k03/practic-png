module;

#include <cstdint>

export module csc.png.png_t;
export import csc.png.png_t.sections;

#ifndef NDEBUG
import csc.stl_wrap.ios;
import csc.stl_wrap.iostream;
#endif
import csc.stl_wrap.variant;
import csc.stl_wrap.vector;

export namespace csc {

class png_t {
 public:
  csc::SUBSCRIBE m_start;
  csc::v_sections m_structured{};
  csc::vector<uint8_t> m_image_data{};
  csc::IHDR m_header;
  csc::IEND m_eof_block;

 public:
  png_t() = default;

  const csc::SUBSCRIBE& start() const {
    return m_start;
  }
  const csc::IHDR& header() const {
    return m_header;
  }
  const csc::IEND& eof_block() const {
    return m_eof_block;
  }

  csc::SUBSCRIBE& start() {
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
csc::ostream& operator<<(csc::ostream& os, const csc::png_t& image) {
  using ct = color_type_t;
  const csc::IHDR& h = csc::get<csc::IHDR>(image.m_structured[0]);
  const char* color_type = (h.color_type() == ct::bw) ? "чёрно-белый"
      : (h.color_type() == ct::rgb)                   ? "цветной"
      : (h.color_type() == ct::indexed)               ? "индексное изображение"
      : (h.color_type() == ct::bwa)                   ? "чёрно-белый (альфа-канал)"
      : (h.color_type() == ct::rgba)                  ? "цветной (альфа-канал)"
                                                      : "unknown";
  const char* interlace = (h.interlace() == interlace_t::none) ? "Отсутствует"
      : (h.interlace() == interlace_t::adam7)                  ? "Есть (Adam7)"
                                                               : "unknown";

  for (uint8_t byte : image.start().data) {
    os << csc::hex << +byte << ' ';
  }
  os << "\nОсновные данные:\n";
  os << "Длина: " << csc::dec << h.width() << " Ширина: " << h.height() << '\n';
  os << "Битовая глубина: " << +h.bit_depth() << '\n';
  os << "Цветность: " << color_type << '\n';

  os << "Метод компрессии: " << ((h.compression() == compression_t::deflate) ? "Алгоритм deflate" : "unknown") << '\n';
  os << "Фильтрация: " << ((h.filter() == filter_t::adaptive) ? "Стандарт (adaptive)" : "unknown") << '\n';
  os << "Поддержка межстрочности: " << interlace << '\n';
  const float image_size_mb = image.m_image_data.size() / 1024.f / 1024.f; 
  os << "Размер изображения в памяти: " << image_size_mb << " Мб.\n";
  return os;
}
#endif
} // namespace csc
