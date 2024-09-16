module;

#include <cstdint>
#include <utility>
#include <bits/allocator.h>
export module csc.png.commons.buffer;

import stl.stl_wrap.stdexcept;
export import :range;

export namespace csc {

template <typename Val>
class basic_buffer_view;
// 4 секунды
template <typename Val>
class basic_buffer {
 public:
  explicit basic_buffer() = default;
  explicit basic_buffer(uint32_t sz) : m_size(sz), m_capacity(sz), m_data(m_alloc.allocate(m_capacity)) {
  }
  basic_buffer(basic_buffer<Val>&& move) noexcept
      : m_alloc(move.m_alloc),
        m_size(std::exchange(move.m_size, 0u)),
        m_capacity(std::exchange(move.m_capacity, 0u)),
        m_data(std::exchange(move.m_data, nullptr)) {
  }
  basic_buffer<Val>& operator=(basic_buffer<Val>&& move) noexcept {
    if (&move == this)
      return *this;
    if (m_data != nullptr || m_capacity == 0u) {
      m_alloc.deallocate(m_data, m_capacity);
    }
    m_data = std::exchange(move.m_data, nullptr);
    m_size = std::exchange(move.m_size, 0u);
    m_capacity = std::exchange(move.m_capacity, 0u);
    m_alloc = move.m_alloc;
    return *this;
  }
  basic_buffer(const basic_buffer<Val>& copy) = delete;
  basic_buffer& operator=(const basic_buffer<Val>& copy) = delete;
  ~basic_buffer() noexcept {
    if (m_data != nullptr || m_capacity == 0u) {
      m_alloc.deallocate(m_data, m_capacity);
    }
  }

 private:
  [[no_unique_address]] std::allocator<Val> m_alloc{};
  uint32_t m_size = 0u;
  uint32_t m_capacity = 0u;
  Val* m_data = nullptr;

 public:
  Val* data() noexcept {
    return m_data;
  }

  const Val* data() const noexcept {
    return m_data;
  }
  uint32_t size() const noexcept {
    return m_size;
  }
  void resize(uint32_t new_size) {
    if (new_size > m_capacity)
      throw std::out_of_range("Осуществлена попытка увеличить размер буфера (new_size > capacity)");
    m_size = new_size; // задаём новый диапазон считываемых значений в текущем буфере
  }
  uint32_t capacity() const noexcept {
    return m_capacity;
  }
  bool is_empty() const noexcept {
    return m_size == 0u;
  }
  Val* begin() {
    return m_data;
  }
  Val* end() {
    return m_data + m_size; // нормальный диапазон
  }
  const Val* begin() const {
    return m_data;
  }
  const Val* end() const {
    return m_data + m_size; // то же самое
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
