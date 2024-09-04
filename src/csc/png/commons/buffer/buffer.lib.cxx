module;

#include <cstdint>
#include <memory>
export module csc.png.commons.buffer;

import cstd.stl_wrap.stdexcept;
export import :range;

export namespace csc {

template <typename Val>
class basic_buffer_view;

template <typename Val>
class basic_buffer {
 public:
  explicit basic_buffer() = default;
  explicit basic_buffer(uint32_t sz) : m_data(std::make_unique<Val[]>(sz)), m_size(sz), m_capacity(sz) {
  }

 private:
  std::unique_ptr<Val[]> m_data = nullptr;
  uint32_t m_size = 0u;
  uint32_t m_capacity = 0u;

 public:
  // move-конструкторы по умолчанию (size и sentinel_size не влияют на освобождаемость m_data
  Val* data() noexcept {
    return m_data.get();
  }

  const Val* data() const noexcept {
    return m_data.get();
  }
  uint32_t size() const noexcept {
    return m_size;
  }
  void resize(uint32_t new_size) {
    if (new_size > m_capacity)
      throw cstd::out_of_range("Осуществлена попытка увеличить размер буфера (new_size > capacity)");
    m_size = new_size; // задаём новый диапазон считываемых значений в текущем буфере
  }
  uint32_t capacity() const noexcept {
    return m_capacity;
  }
  bool is_empty() const noexcept {
    return m_size == 0u;
  }
  Val* begin() {
    return m_data.get();
  }
  Val* end() {
    return m_data.get() + m_size; // нормальный диапазон
  }
  const Val* begin() const {
    return m_data.get();
  }
  const Val* end() const {
    return m_data.get() + m_size; // то же самое
  }
  constexpr operator csc::basic_buffer_view<Val>() const noexcept {
    return csc::basic_buffer_view<Val>(data(), size());
  }
};
using u8buffer = csc::basic_buffer<uint8_t>;
using s8buffer = csc::basic_buffer<int8_t>;
using u16buffer = csc::basic_buffer<uint16_t>;
using s16buffer = csc::basic_buffer<int16_t>;
using u32buffer = csc::basic_buffer<uint32_t>;
using s32buffer = csc::basic_buffer<int32_t>;
using u64buffer = csc::basic_buffer<uint64_t>;
using s64buffer = csc::basic_buffer<int64_t>;

template <typename T>
auto make_buffer(uint32_t size) {
  return csc::basic_buffer<T>(size);
}

} // namespace csc
