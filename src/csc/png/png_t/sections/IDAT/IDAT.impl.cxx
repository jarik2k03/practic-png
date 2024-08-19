module;
#include <cstdint>
#include <ranges>
#include <algorithm>
export module csc.png.png_t.sections.IDAT:impl;

import csc.stl_wrap.vector;
import csc.stl_wrap.stdexcept;
#ifndef NDEBUG
import csc.stl_wrap.iostream;
#endif

export import csc.png.png_t.sections.IHDR; // зависим от состояния header
export import csc.png.png_t.sections.utils;
export import csc.png.png_t.sections.chunk;
export import csc.png.png_t.sections.inflater;

namespace csc {

constexpr uint32_t pixel_size_from_color_type(csc::color_type_t t) {
  using ct = csc::color_type_t;
  switch (t) {
    case ct::rgba:
     return 4u;
    case ct::rgb:
     return 3u;
    case ct::bw:
     return 2u;
    case ct::bwa:
     return 3u;
    case ct::indexed:
     return 1u;
    default:
     return 0u;
  }
}

class IDAT_impl {
 private:
  uint32_t m_crc_adler{0u};

 public:
  IDAT_impl() = default;
  csc::section_code_t do_construct(const csc::chunk& raw, const csc::IHDR& header, csc::inflater& infstream, csc::vector<uint8_t>& generic_image) noexcept;
};

csc::section_code_t
IDAT_impl::do_construct(const csc::chunk& raw, const csc::IHDR& header, csc::inflater& infstream, csc::vector<uint8_t>& generic_image) noexcept {
  try {
    const float pixel_size = (header.bit_depth() / 8.f);
    const uint32_t channels = pixel_size_from_color_type(header.color_type()); 
    generic_image.reserve(static_cast<uint32_t>(header.width() * header.height() * pixel_size * channels * 1.01f)); // после первого резервирования ничего не делает
    infstream.set_compressed_buffer(raw.data);
    do {
      infstream.inflate();
      auto range = infstream.value();
      std::ranges::copy(range, std::back_inserter(generic_image));
    } while (!infstream.done());
  } catch (const csc::runtime_error& e) {
#ifndef NDEBUG
    csc::cerr << "Недесериализован чанк IDAT, причина ошибки: " << e.what() << '\n';
#endif
  }

  m_crc_adler = raw.crc_adler;
  return csc::section_code_t::success;
}

} // namespace csc
