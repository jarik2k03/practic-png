module;
//#include <vector>
#include <bits/stl_construct.h>
#include <string_view>
#include <cstdint>
export module csc.png:deserializer;
import csc.png.png_t;

export namespace csc {

class deserializer {
  public:
  csc::png_t deserialize(std::string_view filepath) {
    csc::IHDR header = IHDR{csc::chunk(), 128u, 128u, 0, color_type_t::rgba, compression_t::deflate, filter_t::adaptive, interlace_t::adam7};
    csc::PLTE palette = PLTE{csc::chunk(), {}};
    csc::IDAT image = IDAT{csc::chunk(), {}};
    csc::IEND eof_block = IEND{csc::chunk() }; 
    return png_t{.m_header = header, .m_palette = palette, .m_image = image, .m_eof_block = eof_block};
  }
};

void print_chunk() {
  csc::chunk ch{};
}

void deserialize() {
}

}
