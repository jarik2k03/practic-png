module;

#include <cstdint>
#include <memory>
export module csc.png.commons.unique_buffer;

export namespace csc {

template <typename It>
struct unique_buf_range {
  It *m_beg, *m_end;
  It* begin() {
    return m_beg;
  }
  It* end() {
    return m_end;
  }
  const It* begin() const {
    return m_beg;
  }
  const It* end() const {
    return m_end;
  }
  unique_buf_range() = delete;
  unique_buf_range(It* b, It* e) : m_beg(b), m_end(e) {
  }
};
using u8unique_buffer_range = csc::unique_buf_range<uint8_t>;
using const_u8unique_buffer_range = csc::unique_buf_range<const uint8_t>;

template <typename Val>
struct basic_unique_buffer {
  basic_unique_buffer() = default;
  basic_unique_buffer(uint32_t sz) : data(std::make_unique<Val[]>(sz)), size(sz) {
  }
  std::unique_ptr<Val[]> data = nullptr;
  uint32_t size = 0u;
  bool is_empty() const {
    return size == 0u;
  }
  Val* begin() {
    return data.get();
  }
  Val* end() {
    return data.get() + size + 1; // возможна ошибка
  }
  const Val* begin() const {
    return data.get();
  }
  const Val* end() const {
    return data.get() + size + 1; // возможна ошибка
  }
};
using u8unique_buffer = csc::basic_unique_buffer<uint8_t>;
using s8unique_buffer = csc::basic_unique_buffer<int8_t>;
using u16unique_buffer = csc::basic_unique_buffer<uint16_t>;
using s16unique_buffer = csc::basic_unique_buffer<int16_t>;
using u32unique_buffer = csc::basic_unique_buffer<uint32_t>;
using s32unique_buffer = csc::basic_unique_buffer<int32_t>;
using u64unique_buffer = csc::basic_unique_buffer<uint64_t>;
using s64unique_buffer = csc::basic_unique_buffer<int64_t>;

template <typename T>
auto make_unique_buffer(uint32_t size) {
  return csc::basic_unique_buffer<T>(size);
}

} // namespace csc
