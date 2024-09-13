module;
#include <cstdint>
#include <zlib.h>
export module csc.png.serializer.produce_chunk.deflater.attributes;

export namespace csc {

enum class e_compression_level : int32_t {
  none = Z_NO_COMPRESSION,
  default_ = Z_DEFAULT_COMPRESSION,
  weakest = Z_BEST_SPEED,
  l1 = 1,
  l2 = 2,
  l3 = 3,
  l4 = 4,
  l5 = 5,
  l6 = 6,
  l7 = 7,
  l8 = 8,
  l9 = 9,
  strongest = Z_BEST_COMPRESSION,
};

enum class e_compression_strategy : int32_t {
  default_ = 0,
  filtered = Z_FILTERED,
  huffman_only = Z_HUFFMAN_ONLY
};

} // namespace csc
