module;
#include <bits/stl_iterator.h>
#include <cstdint>
export module csc.png.picture_debug;

export import csc.png.picture;

import csc.png.picture_debug.write_section_to_ostream;
import cstd.stl_wrap.ios;
import cstd.stl_wrap.ostream;
import cstd.stl_wrap.variant;

export namespace csc {

cstd::ostream& operator<<(cstd::ostream& os, const csc::picture& image) {
  using cstd::operator<<;
  os << "Данные текущего png-файла: \n";
  os << "Сигнатура: \n";
  for (uint8_t byte : image.start().data) {
    os << cstd::hex << +byte << ' ';
  }
  os << '\n';
  const float image_size_mb = image.m_image_data.size() / 1024.f / 1024.f;
  const uint8_t image_bit_depth = cstd::get<csc::IHDR>(image.m_structured[0]).bit_depth;
  os << "Размер изображения в памяти: " << cstd::dec << image_size_mb << " Мб.\n";
  os << "Размер изображения, б: " << image.m_image_data.size() << '\n';
  os << "Ёмкость изображения, б: " << image.m_image_data.capacity() << '\n';

  auto write_section_to_ostream = csc::f_write_section_to_ostream(os, image_bit_depth);
  for (const auto& section : image.m_structured) {
    cstd::visit(write_section_to_ostream, section);
  }
  return os;
}

} // namespace csc
