module;
#include <cstdint>
#include <zlib.h>

module csc.png.deserializer.consume_chunk.inflater:utility;

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
