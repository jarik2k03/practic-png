module;

#include <cstdint>
#include <bits/stl_iterator.h>
#include <type_traits>
export module csc.png.commons.buffer_view;

export import :range;

export namespace csc {
namespace png {

template <typename Val>
class basic_buffer_view {
 private:
  const Val* m_data = nullptr;
  uint32_t m_size = 0u;

 public:
  basic_buffer_view() = default;
  basic_buffer_view(const Val* d, uint32_t s) : m_data(d), m_size(s) {
  }
  // требует contiguious iterator, потому что он превратится в сырой указатель, через который buffer_view должен делать
  // безопасный обход
  template <std::contiguous_iterator Begin, std::contiguous_iterator Sent>
  basic_buffer_view(Begin b, Sent e) {
    m_data = &*b;
    m_size = static_cast<uint32_t>(e - b); // использует random-access возможности
  }
  basic_buffer_view(const Val* d) = delete;
  basic_buffer_view(std::nullptr_t) = delete;

  const Val& operator[](uint32_t idx) const noexcept {
    return m_data[idx];
  }

  const Val* data() const noexcept {
    return m_data;
  }
  uint32_t size() const noexcept {
    return m_size;
  }
};
using u8buffer_view = png::basic_buffer_view<uint8_t>;
using s8buffer_view = png::basic_buffer_view<int8_t>;
using u16buffer_view = png::basic_buffer_view<uint16_t>;
using s16buffer_view = png::basic_buffer_view<int16_t>;
using u32buffer_view = png::basic_buffer_view<uint32_t>;
using s32buffer_view = png::basic_buffer_view<int32_t>;
using u64buffer_view = png::basic_buffer_view<uint64_t>;
using s64buffer_view = png::basic_buffer_view<int64_t>;

} // namespace png
} // namespace csc
