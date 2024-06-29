module;

#include <cstdint>
// #include <string>
#include <array>
#include <vector>
export module csc.png.png_t.sections.chunk;

export namespace csc {
struct chunk {
  uint32_t contained_length = 0u;
  std::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  std::vector<uint8_t> data;
  uint32_t crc_adler = 0u;
};

} // namespace csc
