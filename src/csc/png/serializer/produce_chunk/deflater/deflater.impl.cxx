module;
#include <cstdint>
#include <memory>
#include <utility>
#include <zlib.h>
module csc.png.serializer.produce_chunk.deflater:impl;
import cstd.stl_wrap.stdexcept;
export import csc.png.commons.buffer;
export import csc.png.commons.buffer_view;

import :utility;

namespace csc {

class deflater_impl {
 private:
  int32_t m_state = Z_OK;
  z_stream m_buf_stream = init_z_stream();

  csc::u8buffer_view m_decompressed{}; // input buffer (view)
  csc::u8buffer m_compressed{}; // output buffer

  bool m_is_init = false;

 public:
  deflater_impl() = default;
  ~deflater_impl() noexcept; // implemented
  deflater_impl(const csc::deflater_impl& copy) = delete;
  auto& operator=(const csc::deflater_impl& copy) = delete;
  deflater_impl(csc::deflater_impl&& move) noexcept; // implemented
  deflater_impl& operator=(csc::deflater_impl&& move) noexcept; // implemented

  void do_flush(const csc::u8buffer_view c);
  auto do_value() const;
  void do_deflate(int flush);
  bool do_done() const;
};

deflater_impl::deflater_impl(csc::deflater_impl&& move) noexcept
    : m_state(move.m_state),
      m_buf_stream(move.m_buf_stream),
      m_decompressed(move.m_decompressed),
      m_compressed(std::move(move.m_compressed)),
      m_is_init(std::exchange(move.m_is_init, false)) {
}
deflater_impl::~deflater_impl() noexcept {
  if (m_is_init) {
    deflateEnd(&m_buf_stream);
  }
}
deflater_impl& deflater_impl::operator=(csc::deflater_impl&& move) noexcept {
  if (this == &move)
    return *this;
  if (m_is_init) {
    deflateEnd(&m_buf_stream);
  }
  m_is_init = std::exchange(move.m_is_init, false);
  m_state = move.m_state, m_decompressed = move.m_decompressed;
  m_compressed = std::move(move.m_compressed);
  m_buf_stream = move.m_buf_stream;
  return *this;
}

void deflater_impl::do_deflate(int flush) {
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_compressed.data();
  m_state = deflate(&m_buf_stream, flush);
  if (m_state < 0)
    throw cstd::runtime_error(csc::generate_error_message(m_state));
}
bool deflater_impl::do_done() const {
  return m_buf_stream.avail_in == 0 || m_state == Z_STREAM_END;
}
auto deflater_impl::do_value() const {
  return csc::const_u8buffer_range(m_compressed.begin(), m_compressed.begin() + 16_kB - m_buf_stream.avail_out);
}

void deflater_impl::do_flush(csc::u8buffer_view new_input) {
  if (!m_is_init) {
    m_state = deflateInit(&m_buf_stream, 9);
    if (m_state != Z_OK)
      throw cstd::runtime_error("Не удалось инициализировать deflater!");
    m_compressed = csc::make_buffer<uint8_t>(16_kB);
    m_is_init = true;
  }
  m_decompressed = new_input;
  m_buf_stream.avail_in = m_decompressed.size();
  m_buf_stream.next_in = const_cast<uint8_t*>(m_decompressed.data());
}

} // namespace csc
