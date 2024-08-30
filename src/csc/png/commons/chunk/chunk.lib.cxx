module;

#include <cstdint>
#include <memory>
export module csc.png.commons.chunk;

export import csc.png.commons.unique_buffer;
export import cstd.stl_wrap.array;

export namespace csc {
struct chunk {
  cstd::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  csc::u8unique_buffer buffer{};
  uint32_t crc_adler = 0u;

  uint32_t size = 0u;
  chunk() = default;
  chunk(const chunk& copy) = delete;
  chunk& operator=(const chunk& copy) = delete;
  chunk(chunk&& move) = default;
  chunk& operator=(chunk&& move) = default;
};

} // namespace csc
