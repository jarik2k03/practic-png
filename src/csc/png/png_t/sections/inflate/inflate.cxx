module;
#include <zlib.h>
#include <vector>
#include <cstdint>
export module csc.png.png_t.sections.inflate;


export namespace csc {
constexpr unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024u;
}



std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed, uint32_t opt_buf_size = 128_kB);

}


namespace csc {

std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed, uint32_t opt_buf_size) {
  z_stream buf_stream {0};
  buf_stream.total_in = buf_stream.avail_in = compressed.size();
//   buf_stream.
//   buf_stream
//   buf_stream
//   buf_stream
//   buf_stream
//   buf_stream
//   buf_stream
}


}
