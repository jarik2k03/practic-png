module;
#include <zlib.h>
#include <cstdint>
#include <memory>
module csc.png.png_t.sections.inflater:impl;
export import csc.stl_wrap.vector;
import csc.stl_wrap.stdexcept;
import csc.stl_wrap.iostream;

namespace csc {

constexpr unsigned long long operator"" _kB(unsigned long long koef) {
  return koef * 1024u;
}
constexpr const char* generate_error_message(int32_t status) {
  switch (status) {
   case Z_NEED_DICT:
     return "Отсутствует словарь для декодирования изображения.";
   case Z_BUF_ERROR:
     return "Попытка прочитать данные из пустого или поврежденного буфера.";
   case Z_DATA_ERROR:
     return "Ошибка обработки данных. Данные не извлечены.";
   case Z_VERSION_ERROR:
     return "Неопознанная версия zlib.";
   case Z_MEM_ERROR:
     return "Выход за пределы памяти.";
   case Z_STREAM_ERROR:
     return "Неопознанная степень сжатия.";
   default:
     return "Неизвестная ошибка.";
   }
}

constexpr z_stream init_z_stream() noexcept;

class inflater_impl {
 private:
  int32_t state = Z_OK;
  z_stream m_buf_stream = init_z_stream();
  std::unique_ptr<uint8_t[]> m_uncompressed_buffer{};
  uint32_t m_uncompressed_buffer_size = 0u;
  const csc::vector<uint8_t>& m_compressed;

 public:
  inflater_impl(const csc::vector<uint8_t>& in) : m_compressed(in) {
    state = inflateInit(&m_buf_stream);
    if (state != Z_OK)
      throw csc::runtime_error("Не удалось инициализировать inflater!");
    m_uncompressed_buffer = std::make_unique<uint8_t[]>(16_kB);
    m_buf_stream.avail_in = m_compressed.size();
    m_buf_stream.next_in = const_cast<uint8_t*>(m_compressed.data());

  }
  ~inflater_impl() {
    inflateEnd(&m_buf_stream);
  }

  void do_inflate();
  bool do_done() const;
  auto do_value() const;
};

void inflater_impl::do_inflate() {
  m_buf_stream.avail_out = 16_kB;
  m_buf_stream.next_out = m_uncompressed_buffer.get();
  state = inflate(&m_buf_stream, Z_NO_FLUSH);
  if (state != Z_OK)
    throw csc::runtime_error(csc::generate_error_message(state));
   m_uncompressed_buffer_size = 16_kB - m_buf_stream.avail_out;
}
bool inflater_impl::do_done() const {
  return m_buf_stream.avail_out != 0 || state == Z_STREAM_END;
}
auto inflater_impl::do_value() const {
  return m_uncompressed_buffer.get();
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
