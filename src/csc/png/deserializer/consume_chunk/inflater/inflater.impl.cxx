module;
#include <cstdint>
#include <utility>
#include <bits/alloc_traits.h>
#include <zlib.h>
export module csc.png.deserializer.consume_chunk.inflater:impl;
// без частичного экспорта шаблон не проинстанцируется, и будет undefined reference
import stl.stdexcept;

export import csc.png.commons.buffer;
export import csc.png.commons.buffer_view;
export import csc.png.commons.utility.memory_literals;

import :utility;

namespace csc {

template <typename Alloc>
class inflater_impl {
 private:
  z_stream m_buf_stream = csc::init_z_stream();
  int32_t m_state = Z_OK;

  csc::u8buffer_view m_compressed{}; // input buffer (view)
  csc::u8buffer m_uncompressed{}; // output buffer

  [[no_unique_address]] Alloc m_allocator{}; // аллокатор и хранимые размеры выделенных им блоков памяти

  using u32_alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<uint32_t>;
  csc::stack_alias<uint32_t, u32_alloc> m_allocated_sizes{}; // изменяется только при zalloc и zfree

  csc::allocator_stack_package<Alloc, u32_alloc> m_to_pvoid_package{&m_allocator, &m_allocated_sizes}; // зависим от них

  bool m_is_init = false;

 public:
  inflater_impl(); // implemented
  inflater_impl(const Alloc& alctr);

  ~inflater_impl() noexcept; // implemented
  inflater_impl(const csc::inflater_impl<Alloc>& copy) = delete;
  auto& operator=(const csc::inflater_impl<Alloc>& copy) = delete;
  inflater_impl(csc::inflater_impl<Alloc>&& move) noexcept; // implemented
  inflater_impl& operator=(csc::inflater_impl<Alloc>&& move) noexcept; // implemented

  void do_flush(csc::u8buffer_view c);
  auto do_value() const;
  void do_inflate();
  bool do_done() const;
};

template <typename Alloc>
inflater_impl<Alloc>::inflater_impl() {
  m_buf_stream.opaque = reinterpret_cast<void*>(&m_to_pvoid_package);
  m_buf_stream.zalloc = csc::custom_z_alloc<Alloc, u32_alloc>;
  m_buf_stream.zfree = csc::custom_z_free<Alloc, u32_alloc>;
}
template <typename Alloc>
inflater_impl<Alloc>::inflater_impl(const Alloc& alctr) : m_allocator(alctr) {
  m_buf_stream.opaque = reinterpret_cast<void*>(&m_to_pvoid_package);
  m_buf_stream.zalloc = csc::custom_z_alloc<Alloc, u32_alloc>;
  m_buf_stream.zfree = csc::custom_z_free<Alloc, u32_alloc>;
}

template <typename Alloc>
inflater_impl<Alloc>::inflater_impl(csc::inflater_impl<Alloc>&& move) noexcept
    : m_buf_stream(move.m_buf_stream),
      m_state(move.m_state),
      m_compressed(move.m_compressed),
      m_uncompressed(std::move(move.m_uncompressed)),
      m_allocator(std::move(move.m_allocator)),
      m_allocated_sizes(std::move(move.m_allocated_sizes)),
      m_to_pvoid_package(move.m_to_pvoid_package),
      m_is_init(std::exchange(move.m_is_init, false)) {
}
template <typename Alloc>
inflater_impl<Alloc>::~inflater_impl() noexcept {
  if (m_is_init) {
    inflateEnd(&m_buf_stream);
  }
}
template <typename Alloc>
inflater_impl<Alloc>& inflater_impl<Alloc>::operator=(csc::inflater_impl<Alloc>&& move) noexcept {
  if (this == &move)
    return *this;
  if (m_is_init) {
    inflateEnd(&m_buf_stream);
  }
  m_is_init = std::exchange(move.m_is_init, false);
  m_state = move.m_state, m_compressed = move.m_compressed;
  m_uncompressed = std::move(move.m_uncompressed);
  m_buf_stream = move.m_buf_stream;

  m_allocator = std::move(move.m_allocator);
  m_allocated_sizes = std::move(move.m_allocated_sizes);
  m_to_pvoid_package = move.m_to_pvoid_package;
  return *this;
}

template <typename Alloc>
void inflater_impl<Alloc>::do_inflate() {
  using namespace csc::memory_literals;
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_uncompressed.data();
  m_state = inflate(&m_buf_stream, Z_NO_FLUSH);

  if (m_state < 0)
    throw std::runtime_error(csc::generate_error_message(m_state));
}
template <typename Alloc>
bool inflater_impl<Alloc>::do_done() const {
  using namespace csc::memory_literals;
  return m_buf_stream.avail_in == 0_B || m_state == Z_STREAM_END;
}
template <typename Alloc>
auto inflater_impl<Alloc>::do_value() const {
  using namespace csc::memory_literals;
  return csc::const_u8buffer_range(m_uncompressed.begin(), m_uncompressed.begin() + 16_kB - m_buf_stream.avail_out);
}

template <typename Alloc>
void inflater_impl<Alloc>::do_flush(csc::u8buffer_view change) {
  using namespace csc::memory_literals;
  if (!m_is_init) {
    m_state = inflateInit(&m_buf_stream);
    if (m_state != Z_OK)
      throw std::runtime_error("Не удалось инициализировать inflater!");
    m_uncompressed = csc::make_buffer<uint8_t>(16_kB);
    m_is_init = true;
  }
  m_compressed = change; // very cheap copy-operator=
  m_buf_stream.avail_in = m_compressed.size();
  m_buf_stream.next_in = const_cast<uint8_t*>(m_compressed.data());
}

} // namespace csc
