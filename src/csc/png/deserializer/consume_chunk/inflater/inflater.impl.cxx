module;
#include <cstdint>
#include <memory>
#include <utility>
#include <zlib.h>
module csc.png.deserializer.consume_chunk.inflater:impl;
import cstd.stl_wrap.stdexcept;
export import csc.png.commons.unique_buffer;

export import :attributes;

namespace csc {

constexpr unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024u;
}
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

class inflater_impl {
 private:
  int32_t m_state = Z_OK;
  z_stream m_buf_stream = init_z_stream();
  csc::u8unique_buffer m_uncompressed{};
  const csc::u8unique_buffer* m_compressed{};
  bool m_is_init = false;

 public:
  inflater_impl() = default;
  ~inflater_impl() noexcept; // implemented
  inflater_impl(const csc::inflater_impl& copy) = delete;
  auto& operator=(const csc::inflater_impl& copy) = delete;
  inflater_impl(csc::inflater_impl&& move) noexcept; // implemented
  inflater_impl& operator=(csc::inflater_impl&& move) noexcept; // implemented

  void do_set_compressed_buffer(const csc::u8unique_buffer& c);
  auto do_value() const;
  void do_inflate();
  bool do_done() const;
};

inflater_impl::inflater_impl(csc::inflater_impl&& move) noexcept
    : m_state(move.m_state),
      m_buf_stream(move.m_buf_stream),
      m_uncompressed(std::move(move.m_uncompressed)),
      m_compressed(move.m_compressed),
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

void inflater_impl::do_inflate() {
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_uncompressed.data.get();
  m_state = inflate(&m_buf_stream, Z_NO_FLUSH);

  if (m_state < 0)
    throw cstd::runtime_error(csc::generate_error_message(m_state));
}
bool inflater_impl::do_done() const {
  return m_buf_stream.avail_in == 0 || m_buf_stream.avail_out != 0 || m_state == Z_STREAM_END;
}
auto inflater_impl::do_value() const {
  return csc::const_u8unique_buffer_range(
      m_uncompressed.begin(), m_uncompressed.begin() + 16_kB - m_buf_stream.avail_out);
}

void inflater_impl::do_set_compressed_buffer(const csc::u8unique_buffer& c) {
  if (!m_is_init) {
    m_state = inflateInit(&m_buf_stream);
    if (m_state != Z_OK)
      throw cstd::runtime_error("Не удалось инициализировать inflater!");
    m_uncompressed = csc::make_unique_buffer<uint8_t>(16_kB);
    m_is_init = true;
  }
  m_compressed = &c;
  m_buf_stream.avail_in = m_compressed->size;
  m_buf_stream.next_in = const_cast<uint8_t*>(m_compressed->data.get());
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
