module;
#include <cstdint>
#include <memory>
#include <utility>
#include <zlib.h>
module csc.png.deserializer.consume_chunk.inflater:impl;
import cstd.stl_wrap.stdexcept;
export import csc.png.commons.buffer;
export import csc.png.commons.buffer_view;

import :utility;

namespace csc {

class inflater_impl {
 private:
  int32_t m_state = Z_OK;
  z_stream m_buf_stream = init_z_stream();

  csc::u8buffer_view m_compressed{}; // input buffer (view)
  csc::u8buffer m_uncompressed{}; // output buffer

  bool m_is_init = false;

 public:
  inflater_impl() = default;
  ~inflater_impl() noexcept; // implemented
  inflater_impl(const csc::inflater_impl& copy) = delete;
  auto& operator=(const csc::inflater_impl& copy) = delete;
  inflater_impl(csc::inflater_impl&& move) noexcept; // implemented
  inflater_impl& operator=(csc::inflater_impl&& move) noexcept; // implemented

  void do_flush(csc::u8buffer_view c);
  auto do_value() const;
  void do_inflate(int flush);
  bool do_done() const;
};

inflater_impl::inflater_impl(csc::inflater_impl&& move) noexcept
    : m_state(move.m_state),
      m_buf_stream(move.m_buf_stream),
      m_compressed(move.m_compressed),
      m_uncompressed(std::move(move.m_uncompressed)),
      m_is_init(std::exchange(move.m_is_init, false)) {
}
inflater_impl::~inflater_impl() noexcept {
  if (m_is_init) {
    inflateEnd(&m_buf_stream);
  }
}
inflater_impl& inflater_impl::operator=(csc::inflater_impl&& move) noexcept {
  if (this == &move)
    return *this;
  if (m_is_init) {
    inflateEnd(&m_buf_stream);
  }
  m_is_init = std::exchange(move.m_is_init, false);
  m_state = move.m_state, m_compressed = move.m_compressed;
  m_uncompressed = std::move(move.m_uncompressed);
  m_buf_stream = move.m_buf_stream;
  return *this;
}

void inflater_impl::do_inflate(int flush) {
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_uncompressed.data();
  m_state = inflate(&m_buf_stream, flush);

  if (m_state < 0)
    throw cstd::runtime_error(csc::generate_error_message(m_state));
}
bool inflater_impl::do_done() const {
  return m_buf_stream.avail_in == 0 || m_state == Z_STREAM_END;
}
auto inflater_impl::do_value() const {
  return csc::const_u8buffer_range(m_uncompressed.begin(), m_uncompressed.begin() + 16_kB - m_buf_stream.avail_out);
}

void inflater_impl::do_flush(csc::u8buffer_view change) {
  if (!m_is_init) {
    m_state = inflateInit(&m_buf_stream);
    if (m_state != Z_OK)
      throw cstd::runtime_error("Не удалось инициализировать inflater!");
    m_uncompressed = csc::make_buffer<uint8_t>(16_kB);
    m_is_init = true;
  }
  m_compressed = change; // very cheap copy-operator=
  m_buf_stream.avail_in = m_compressed.size();
  m_buf_stream.next_in = const_cast<uint8_t*>(m_compressed.data());
}

} // namespace csc
