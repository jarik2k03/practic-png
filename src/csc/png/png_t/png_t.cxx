module;

#ifndef NDEBUG
#include <ostream>
#include <cstdint>
#endif

export module csc.png.png_t;
export import csc.png.png_t.sections;

export namespace csc {

class png_t {
 public:
  csc::SUBSCRIBE m_start{};
  csc::IHDR m_header;
  csc::PLTE m_palette;
  csc::IDAT m_image;
  csc::IEND m_eof_block;
 public:

  const csc::SUBSCRIBE& start() const { return m_start; }
  const csc::IHDR& header() const { return m_header; }
  const csc::PLTE& palette() const { return m_palette; }
  const csc::IDAT& image() const { return m_image; }
  const csc::IEND& eof_block() const { return m_eof_block; }

  csc::SUBSCRIBE& start() { return m_start; }
  csc::IHDR& header()  { return m_header; }
  csc::PLTE& palette()  { return m_palette; }
  csc::IDAT& image()  { return m_image; }
  csc::IEND& eof_block()  { return m_eof_block; }

};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const png_t& image) {
  for (uint8_t byte : image.start().data) {
    os << std::hex << byte << ' ';
  }
  os << "\nОсновные данные:\n";
  const csc::IHDR& h = image.header();
  os << "Длина: " << h.width << " Ширина: " << h.height << '\n';
  os << "Битовая глубина: " << h.bit_depth * 8 << '\n';
  using ct = color_type_t;
  const char* color_type = (h.color_type == ct::bw) ? "чёрно-белый" :
    (h.color_type == ct::rgb) ? "цветной" : (h.color_type == ct::indexed) ? "индексное изображение" :
    (h.color_type == ct::bwa) ? "чёрно-белый (альфа-канал)" : (h.color_type == ct::rgba) ? "цветной (альфа-канал)" : "unknown";
  os << "Цветность: " << color_type << '\n';

  os << "Метод компрессии: " << (h.compression == compression_t::deflate) ? "Алгоритм deflate" : "unknown";
  os << "Фильтрация: " <<  (h.filter == filter_t::adaptive) ? "Стандарт (adaptive)" : "unknown";
  const char* interlace = (h.interlace == interlace_t::none) ? "Отсутствует" :
    (h.interlace == interlace_t::adam7) ? "Есть (Adam7)" : "unknown"; 
  os << "Поддержка межстрочности: " << interlace << '\n';

  return os;
}
#endif
}
