module;
#include <array>
#include <vector>
#include <tuple>
#include <cstdint>
export module csc.png.sections;

export import :chunk;
export import :attributes;

export namespace csc {

struct subscribe {
  constexpr static const std::array<uint8_t, 8> data = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
};

struct IHDR : public chunk {
  uint32_t width, height;
  uint8_t bit_depth;
  csc::color_type_t color_type;
  csc::compression_t compression;
  csc::filter_t filter; 
  csc::interlace_t interlace; 
};

struct PLTE : public chunk {
  using rgb8 = std::tuple<uint8_t, uint8_t, uint8_t>;
  std::array<rgb8, 256> full_palette;
  uint8_t full_palette_size = 0;
};

struct IDAT : public chunk {
  std::vector<uint8_t> compressed_data;
};
struct IEND : public chunk {
};




}
