module;
#include <cstdint>
#include <utility>
#include <bits/alloc_traits.h>
#include <zlib.h>
export module csc.png.serializer.produce_chunk.deflater:impl;
// без частичного экспорта шаблон не проинстанцируется, и будет undefined reference

import stl.stdexcept;

export import csc.png.serializer.produce_chunk.deflater.attributes;
export import csc.png.commons.buffer;
export import csc.png.commons.buffer_view;
export import csc.png.commons.utility.memory_literals;

import :utility;

namespace csc {
namespace png {

template <typename Alloc>
class deflater_impl {
 private:
  png::u8buffer_view m_decompressed{}; // input buffer (view)
  png::u8buffer m_compressed{}; // output buffer

  z_stream m_buf_stream = png::init_z_stream();
  int32_t m_state = Z_OK;
  png::e_compression_level m_compr_level = e_compression_level::default_;
  png::e_compression_strategy m_strategy = e_compression_strategy::default_;
  int32_t m_mem_level = 8, m_win_bits = 15;

  [[no_unique_address]] Alloc m_allocator{}; // аллокатор и хранимые размеры выделенных им блоков памяти

  using u32_alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<uint32_t>;
  png::stack_alias<uint32_t, u32_alloc> m_allocated_sizes{}; // изменяется только при zalloc и zfree

  png::allocator_stack_package<Alloc, u32_alloc> m_to_pvoid_package{&m_allocator, &m_allocated_sizes}; // зависим от них

  bool m_is_init = false;

 public:
  deflater_impl() = default;
  deflater_impl(const Alloc& alctr);
  deflater_impl(png::e_compression_level c, png::e_compression_strategy s, int32_t m, int32_t w);

  ~deflater_impl() noexcept; // implemented
  deflater_impl(const png::deflater_impl<Alloc>& copy) = delete;
  auto& operator=(const png::deflater_impl<Alloc>& copy) = delete;
  deflater_impl(png::deflater_impl<Alloc>&& move) noexcept; // implemented
  deflater_impl& operator=(png::deflater_impl<Alloc>&& move) noexcept; // implemented

  void do_flush(const png::u8buffer_view c);
  auto do_value() const;
  void do_deflate(uint32_t stride_read);
  bool do_done() const;
};

template <typename Alloc>
deflater_impl<Alloc>::deflater_impl(png::e_compression_level c, png::e_compression_strategy s, int32_t m, int32_t w)
    : m_compr_level(c), m_strategy(s), m_mem_level(m), m_win_bits(w) {
  m_buf_stream.opaque = reinterpret_cast<void*>(&m_to_pvoid_package);
  m_buf_stream.zalloc = png::custom_z_alloc<Alloc, u32_alloc>;
  m_buf_stream.zfree = png::custom_z_free<Alloc, u32_alloc>;
}

template <typename Alloc>
deflater_impl<Alloc>::deflater_impl(const Alloc& alctr) : m_allocator(alctr) {
  m_buf_stream.opaque = reinterpret_cast<void*>(&m_to_pvoid_package);
  m_buf_stream.zalloc = png::custom_z_alloc<Alloc, u32_alloc>;
  m_buf_stream.zfree = png::custom_z_free<Alloc, u32_alloc>;
}

template <typename Alloc>
deflater_impl<Alloc>::deflater_impl(png::deflater_impl<Alloc>&& move) noexcept
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
      m_to_pvoid_package(move.m_to_pvoid_package),
      m_is_init(std::exchange(move.m_is_init, false)) {
}

template <typename Alloc>
deflater_impl<Alloc>::~deflater_impl() noexcept {
  if (m_is_init) {
    deflateEnd(&m_buf_stream);
  }
}
template <typename Alloc>
deflater_impl<Alloc>& deflater_impl<Alloc>::operator=(png::deflater_impl<Alloc>&& move) noexcept {
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
  m_to_pvoid_package = move.m_to_pvoid_package;
  m_buf_stream = move.m_buf_stream;
  m_compr_level = move.m_compr_level;
  m_strategy = move.m_strategy;
  m_mem_level = move.m_mem_level;
  m_win_bits = move.m_win_bits;
  return *this;
}

template <typename Alloc>
void deflater_impl<Alloc>::do_deflate(uint32_t stride_read) {
  using namespace png::memory_literals;
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_compressed.data();

  m_state = deflate(&m_buf_stream, (m_decompressed.size() == stride_read) ? Z_SYNC_FLUSH : Z_FINISH);
  if (m_state < 0)
    throw std::runtime_error(png::generate_error_message(m_state));
}
template <typename Alloc>
bool deflater_impl<Alloc>::do_done() const {
  using namespace png::memory_literals;
  return m_state == Z_STREAM_END || m_buf_stream.avail_out != 0_B;
}
template <typename Alloc>
auto deflater_impl<Alloc>::do_value() const {
  using namespace png::memory_literals;
  return png::const_u8buffer_range(m_compressed.begin(), m_compressed.begin() + 16_kB - m_buf_stream.avail_out);
}

template <typename Alloc>
void deflater_impl<Alloc>::do_flush(png::u8buffer_view new_input) {
  using namespace png::memory_literals;
  if (!m_is_init) {
    m_state = deflateInit2(
        &m_buf_stream,
        static_cast<int32_t>(m_compr_level),
        Z_DEFLATED,
        m_win_bits,
        m_mem_level,
        static_cast<int32_t>(m_strategy));
    if (m_state != Z_OK)
      throw std::runtime_error("Не удалось инициализировать deflater!");
    m_compressed = png::make_buffer<uint8_t>(16_kB);
    m_is_init = true;
  }
  m_decompressed = new_input;
  m_buf_stream.avail_in = m_decompressed.size();
  m_buf_stream.next_in = const_cast<uint8_t*>(m_decompressed.data());
}

} // namespace png
} // namespace csc
