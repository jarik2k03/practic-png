module;
#include <zlib.h>
#include <cstdint>
module csc.png.png_t.sections.inflater:impl;
export import csc.stl_wrap.vector;

namespace csc {

constexpr z_stream init_z_stream() noexcept;

class inflater_impl {
 private:
  z_stream m_buf_stream;
  const csc::vector<uint8_t>& m_compressed;

 public:
  inflater_impl(const csc::vector<uint8_t>& in) : m_buf_stream(init_z_stream()), m_compressed(in) {
    const int result = inflateInit(&m_buf_stream);
  }
  ~inflater_impl() {
    inflateEnd(&m_buf_stream);
  }

  void do_inflate(int flush);
};

void inflater_impl::do_inflate(int flush) {
  csc::vector<uint8_t> out;
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

} // namespace csc