module;

#include <cstdint>
//#include <string>

#include <array>
#include <vector>
export module csc.png.sections:chunk;

export namespace csc {
class chunk {
  uint32_t contained_length = 0u;
  std::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  std::vector<char> contained{};
  uint32_t crc_adler = 0u;
};


}
