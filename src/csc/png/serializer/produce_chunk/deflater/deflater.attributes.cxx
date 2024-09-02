module;
#include <cstdint>
export module csc.png.serializer.produce_chunk.deflater:attributes;

export namespace csc {

constexpr unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024u;
}

} // namespace csc
