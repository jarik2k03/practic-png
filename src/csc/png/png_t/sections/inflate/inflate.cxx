module;
#include <zlib.h>
#include <vector>
#include <cstdint>
#include <stdexcept>
export module csc.png.png_t.sections.inflate;



namespace csc {
constexpr z_stream init_z_stream() noexcept; 
}

export namespace csc {


class inflater {
 private:
  z_stream m_buf_stream;
  const std::vector<uint8_t>& m_compressed;
 public:
  inflater(const std::vector<uint8_t>& in) : m_buf_stream(init_z_stream()), m_compressed(in) {
    const int result = inflateInit(&m_buf_stream);
    if (result != Z_OK)
      throw std::runtime_error("Не удалось создать объект inflater!"); 
  }
  ~inflater() {
    inflateEnd(&m_buf_stream);
  }

  void inflate(int flush);   
};

constexpr unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024u;
}

}


namespace csc {


void inflater::inflate(int flush) {
  std::vector<uint8_t> out;

}



constexpr z_stream init_z_stream() noexcept {
  z_stream st;
  st.zalloc = Z_NULL;
  st.zfree = Z_NULL;
  st.opaque = Z_NULL;
  st.next_in = st.next_out = Z_NULL;
  st.total_in = st.total_out = st.avail_in = st.avail_out = 0u;
  return st;
}

}
