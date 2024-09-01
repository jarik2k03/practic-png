module;
#include <cstdint>
#include <memory>
#include <utility>
#include <zlib.h>
module csc.png.serializer.produce_chunk.deflater:impl;
import cstd.stl_wrap.stdexcept;
export import csc.png.commons.unique_buffer;
export import cstd.stl_wrap.vector;

export import :attributes;

namespace csc {

constexpr const char* generate_error_message(int32_t status) {
  switch (status) {
    case Z_NEED_DICT:
      return "Отсутствует словарь для декодирования изображения.";
    case Z_STREAM_END:
      return "Попытка чтения из остановленного потока.";
    case Z_BUF_ERROR:
      return "Попытка записать данные в переполненный буфер.";
    case Z_DATA_ERROR:
      return "Вероятность повреждения данных. Входной поток не соответствует "
             "формату zlib";
    case Z_VERSION_ERROR:
      return "Неопознанная версия zlib.";
    case Z_MEM_ERROR:
      return "Выход за пределы памяти.";
    case Z_STREAM_ERROR:
      return "Неопознанная степень сжатия или структура потока неправильно "
             "инициализирована.";
    default:
      return "Неизвестная ошибка.";
  }
}

constexpr z_stream init_z_stream() noexcept;

class deflater_impl {
 private:
  int32_t m_state = Z_OK;
  z_stream m_buf_stream = init_z_stream();
  csc::u8unique_buffer m_compressed{};
  const cstd::vector<uint8_t>* m_decompressed{};
  bool m_is_init = false;

 public:
  deflater_impl() = default;
  ~deflater_impl() noexcept; // implemented
  deflater_impl(const csc::deflater_impl& copy) = delete;
  auto& operator=(const csc::deflater_impl& copy) = delete;
  deflater_impl(csc::deflater_impl&& move) noexcept; // implemented
  deflater_impl& operator=(csc::deflater_impl&& move) noexcept; // implemented

  void do_set_decompressed_buffer(const cstd::vector<uint8_t>& c);
  auto do_value() const;
  void do_deflate();
  bool do_done() const;
};

deflater_impl::deflater_impl(csc::deflater_impl&& move) noexcept
    : m_state(move.m_state),
      m_buf_stream(move.m_buf_stream),
      m_compressed(std::move(move.m_compressed)),
      m_decompressed(move.m_decompressed),
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

void deflater_impl::do_deflate() {
  m_buf_stream.avail_out = 8_kB;
  m_buf_stream.next_out = m_compressed.data.get();
  m_state = deflate(&m_buf_stream, Z_NO_FLUSH);
  if (m_state < 0)
    throw cstd::runtime_error(csc::generate_error_message(m_state));
}
bool deflater_impl::do_done() const {
  return m_buf_stream.avail_in == 0 || m_buf_stream.avail_out != 0 || m_state == Z_STREAM_END;
}
auto deflater_impl::do_value() const {
  return csc::const_u8unique_buffer_range(m_compressed.begin(), m_compressed.begin() + 8_kB - m_buf_stream.avail_out);
}

void deflater_impl::do_set_decompressed_buffer(const cstd::vector<uint8_t>& c) {
  if (!m_is_init) {
    m_state = deflateInit(&m_buf_stream, Z_DEFAULT_COMPRESSION);
    if (m_state != Z_OK)
      throw cstd::runtime_error("Не удалось инициализировать deflater!");
    m_compressed = csc::make_unique_buffer<uint8_t>(8_kB);
    m_is_init = true;
  }
  m_decompressed = &c;
  m_buf_stream.avail_in = m_decompressed->size();
  m_buf_stream.next_in = const_cast<uint8_t*>(m_decompressed->data());
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
