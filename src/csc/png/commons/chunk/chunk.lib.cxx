module;

#include <cstdint>
export module csc.png.commons.chunk;
export import :attributes;

export import csc.png.commons.buffer;
export import cstd.stl_wrap.array;

export namespace csc {

struct chunk {
  cstd::array<char, 4> chunk_name = {'\0', '\0', '\0', '\0'};
  csc::u8buffer buffer{};
  uint32_t crc_adler = 0u;

  chunk() = default;
  chunk(const chunk& copy) = delete;
  chunk& operator=(const chunk& copy) = delete;
  chunk(chunk&& move) = default;
  chunk& operator=(chunk&& move) = default;

  inline bool operator==(const csc::chunk& two) const {
    using cstd::operator==; // благодаря контрольным суммам нет необходимости
                            // сравнивать буферы друг с другом
    return this->chunk_name == two.chunk_name && this->crc_adler == two.crc_adler;
  }
  inline bool operator!=(const csc::chunk& two) const {
    return !(*this == two);
  }
};

} // namespace csc
