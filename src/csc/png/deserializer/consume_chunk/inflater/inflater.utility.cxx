module;
#include <cstdint>
#include <zlib.h>
#include <bits/allocator.h>
#include <stack>

export module csc.png.deserializer.consume_chunk.inflater:utility;
// без частичного экспорта шаблон не проинстанцируется, и будет undefined reference

namespace csc {
namespace png {

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

template <typename Val, typename Alloc2>
using stack_alias = std::stack<Val, std::deque<Val, Alloc2>>;

template <typename Alloc1, typename Alloc2>
using allocator_stack_package = std::pair<Alloc1*, png::stack_alias<uint32_t, Alloc2>*>;

template <typename Alloc1, typename Alloc2>
void* custom_z_alloc(void* opq, uint32_t cnt, uint32_t size) noexcept {
  auto& external_data = *reinterpret_cast<allocator_stack_package<Alloc1, Alloc2>*>(opq);
  external_data.second->push(cnt * size);
  return external_data.first->allocate(cnt * size);
}

template <typename Alloc1, typename Alloc2>
void custom_z_free(void* opq, void* resource) noexcept {
  auto& external_data = *reinterpret_cast<allocator_stack_package<Alloc1, Alloc2>*>(opq);
  auto allocated_size = external_data.second->top();
  external_data.second->pop();
  using resource_type = std::allocator_traits<Alloc1>::value_type;
  external_data.first->deallocate(reinterpret_cast<resource_type*>(resource), allocated_size);
}

} // namespace png
} // namespace csc
