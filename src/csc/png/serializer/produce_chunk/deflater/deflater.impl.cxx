module;
#include <cstdint>
#include <utility>
#include <zlib.h>
export module csc.png.serializer.produce_chunk.deflater:impl;
// без частичного экспорта шаблон не проинстанцируется, и будет undefined reference

import cstd.stl_wrap.stdexcept;

export import csc.png.serializer.produce_chunk.deflater.attributes;
export import csc.png.commons.buffer;
export import csc.png.commons.buffer_view;

import :utility;

namespace csc {

template <typename Alloc>
class deflater_impl {
 private:
  csc::u8buffer_view m_decompressed{}; // input buffer (view)
  csc::u8buffer m_compressed{}; // output buffer

  z_stream m_buf_stream = csc::init_z_stream();
  int32_t m_state = Z_OK;
  csc::e_compression_level m_compr_level = e_compression_level::default_;
  csc::e_compression_strategy m_strategy = e_compression_strategy::default_;
  int32_t m_mem_level = 8, m_win_bits = 15;

  [[no_unique_address]] Alloc m_allocator{}; // аллокатор и хранимые размеры выделенных им блоков памяти
  csc::stack_alias<uint32_t, Alloc> m_allocated_sizes{}; // изменяется только при zalloc и zfree

  csc::allocator_stack_package<Alloc> m_to_pvoid_package{&m_allocator, &m_allocated_sizes}; // зависим от них

  bool m_is_init = false;

 public:
  deflater_impl() = default;
  deflater_impl(const Alloc& alctr);
  deflater_impl(csc::e_compression_level c, csc::e_compression_strategy s, int32_t m, int32_t w);

  ~deflater_impl() noexcept; // implemented
  deflater_impl(const csc::deflater_impl<Alloc>& copy) = delete;
  auto& operator=(const csc::deflater_impl<Alloc>& copy) = delete;
  deflater_impl(csc::deflater_impl<Alloc>&& move) noexcept; // implemented
  deflater_impl& operator=(csc::deflater_impl<Alloc>&& move) noexcept; // implemented

  void do_flush(const csc::u8buffer_view c);
  auto do_value() const;
  void do_deflate(uint32_t stride_read);
  bool do_done() const;
};

template <typename Alloc>
deflater_impl<Alloc>::deflater_impl(csc::e_compression_level c, csc::e_compression_strategy s, int32_t m, int32_t w)
    : m_compr_level(c), m_strategy(s), m_mem_level(m), m_win_bits(w) {
  m_buf_stream.opaque = reinterpret_cast<void*>(&m_to_pvoid_package);
  m_buf_stream.zalloc = csc::custom_z_alloc<Alloc>;
  m_buf_stream.zfree = csc::custom_z_free<Alloc>;
}

template <typename Alloc>
deflater_impl<Alloc>::deflater_impl(const Alloc& alctr) : m_allocator(alctr) {
  m_buf_stream.opaque = reinterpret_cast<void*>(&m_to_pvoid_package);
  m_buf_stream.zalloc = csc::custom_z_alloc<Alloc>;
  m_buf_stream.zfree = csc::custom_z_free<Alloc>;
}

template <typename Alloc>
deflater_impl<Alloc>::deflater_impl(csc::deflater_impl<Alloc>&& move) noexcept
    : m_decompressed(move.m_decompressed),
      m_compressed(std::move(move.m_compressed)),
      m_buf_stream(move.m_buf_stream),
      m_state(move.m_state),
      m_compr_level(move.m_compr_level),
      m_strategy(move.m_strategy),
      m_mem_level(move.m_mem_level),
      m_win_bits(move.m_win_bits),
      m_allocator(std::move(move.m_allocator)),
      m_allocated_sizes(std::move(move.m_allocated_sizes)),
      m_is_init(std::exchange(move.m_is_init, false)) {
}

template <typename Alloc>
deflater_impl<Alloc>::~deflater_impl() noexcept {
  if (m_is_init) {
    deflateEnd(&m_buf_stream);
  }
}
template <typename Alloc>
deflater_impl<Alloc>& deflater_impl<Alloc>::operator=(csc::deflater_impl<Alloc>&& move) noexcept {
  if (this == &move)
    return *this;
  if (m_is_init) {
    deflateEnd(&m_buf_stream);
  }
  m_is_init = std::exchange(move.m_is_init, false);
  m_state = move.m_state, m_decompressed = move.m_decompressed;
  m_compressed = std::move(move.m_compressed);
  m_allocator = std::move(move.m_allocator);
  m_allocated_sizes = std::move(move.m_allocated_sizes);
  m_buf_stream = move.m_buf_stream;
  m_compr_level = move.m_compr_level;
  m_strategy = move.m_strategy;
  m_mem_level = move.m_mem_level;
  m_win_bits = move.m_win_bits;
  return *this;
}

template <typename Alloc>
void deflater_impl<Alloc>::do_deflate(uint32_t stride_read) {
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_compressed.data();

  m_state = deflate(&m_buf_stream, (m_decompressed.size() == stride_read) ? Z_SYNC_FLUSH : Z_FINISH);
  if (m_state < 0)
    throw cstd::runtime_error(csc::generate_error_message(m_state));
}
template <typename Alloc>
bool deflater_impl<Alloc>::do_done() const {
  return m_state == Z_STREAM_END || m_buf_stream.avail_out != 0;
}
template <typename Alloc>
auto deflater_impl<Alloc>::do_value() const {
  return csc::const_u8buffer_range(m_compressed.begin(), m_compressed.begin() + 16_kB - m_buf_stream.avail_out);
}

template <typename Alloc>
void deflater_impl<Alloc>::do_flush(csc::u8buffer_view new_input) {
  if (!m_is_init) {
    m_state = deflateInit2(
        &m_buf_stream,
        static_cast<int32_t>(m_compr_level),
        Z_DEFLATED,
        m_win_bits,
        m_mem_level,
        static_cast<int32_t>(m_strategy));
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
