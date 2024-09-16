module;
#include <bits/stl_iterator.h>
#include <cstdint>
export module csc.png.picture_debug;

export import csc.png.picture;

import csc.png.picture_debug.write_section_to_ostream;
import stl.ios;
import stl.ostream;
import stl.variant;

export namespace csc {

std::ostream& operator<<(std::ostream& os, const csc::picture& image) {
  using std::operator<<;
  os << "Данные текущего png-файла: \n";
  os << "Сигнатура: \n";
  for (uint8_t byte : image.start().data) {
    os << std::hex << +byte << ' ';
  }
  os << '\n';
  const float image_size_mb = image.m_image_data.size() / 1024.f / 1024.f;
  const uint8_t image_bit_depth = std::get<csc::IHDR>(image.m_structured[0]).bit_depth;
  os << "Размер изображения в памяти: " << std::dec << image_size_mb << " Мб.\n";
  os << "Размер изображения, б: " << image.m_image_data.size() << '\n';
  os << "Ёмкость изображения, б: " << image.m_image_data.capacity() << '\n';

  auto write_section_to_ostream = csc::f_write_section_to_ostream(os, image_bit_depth);
  for (const auto& section : image.m_structured) {
    std::visit(write_section_to_ostream, section);
  }
  return os;
}

} // namespace csc
