module;

#include <cstdint>
#include <cstring>
#include <cmath>
export module csc.png.commons.filtering;

export import :attributes;

export namespace csc {
namespace png {

class filtering {
 private:
  uint32_t m_pixel_bytesize;
  uint32_t m_linebytes_width;

 public:
  filtering() = default;
  filtering(uint32_t pixel_bytesize, uint32_t linebytes_width)
      : m_pixel_bytesize(pixel_bytesize), m_linebytes_width(linebytes_width) {
  }
  uint32_t get_linebytes_width(uint32_t clamped_bit_depth) const noexcept;
  uint32_t get_pixel_bytesize() const noexcept;

  void none(uint8_t* to, const uint8_t* current) const noexcept;
  void sub(uint8_t* to, const uint8_t* current) const noexcept;
  void up(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void average(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void average(uint8_t* to, const uint8_t* current) const noexcept; // частный случай
  void paeth(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept;
  void paeth(uint8_t* to, const uint8_t* current) const noexcept;
};

uint32_t filtering::get_linebytes_width(uint32_t clamped_bit_depth) const noexcept {
  return static_cast<uint32_t>(::ceilf(static_cast<float>(m_linebytes_width) / (8u / clamped_bit_depth)));
}
uint32_t filtering::get_pixel_bytesize() const noexcept {
  return m_pixel_bytesize;
}

void filtering::none(uint8_t* to, const uint8_t* current) const noexcept {
  for (uint32_t i = 0u; i < m_linebytes_width; ++i) {
    to[i] = current[i];
  }
}

void filtering::sub(uint8_t* to, const uint8_t* current) const noexcept {
  ::memcpy(to, current, m_pixel_bytesize); // копируем первый пиксель
  for (uint32_t i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    to[i] = current[i] + recon_a;
  }
}

void filtering::up(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept {
  for (auto i = 0u; i < m_linebytes_width; ++i) {
    const auto recon_b = previous[i];
    to[i] = current[i] + recon_b;
  }
}

void filtering::average(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    const auto recon_b = previous[i];
    to[i] = current[i] + average_arithmetic(0u, recon_b);
  } // первый пиксель в строке не учитывает предыдущие

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    const auto recon_b = previous[i];
    to[i] = current[i] + static_cast<uint8_t>(png::average_arithmetic(recon_a, recon_b));
  }
}

void filtering::average(uint8_t* to, const uint8_t* current) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    to[i] = current[i] + average_arithmetic(0u, 0u);
  } // первый пиксель в строке не учитывает предыдущие

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    to[i] = current[i] + static_cast<uint8_t>(png::average_arithmetic(recon_a, 0u)); // recon_b замещает пустой байт
  }
}

void filtering::paeth(uint8_t* to, const uint8_t* current, const uint8_t* previous) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    const auto recon_b = previous[i];
    to[i] = current[i] + png::paeth_predictor(0u, recon_b, 0u);
  } // первый пиксель в строке не учитывает предыдущие

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];
    const auto recon_b = previous[i];
    const auto recon_c = previous[i - m_pixel_bytesize];

    to[i] = current[i] + png::paeth_predictor(recon_a, recon_b, recon_c);
  }
}

void filtering::paeth(uint8_t* to, const uint8_t* current) const noexcept {
  for (auto i = 0u; i < m_pixel_bytesize; ++i) {
    to[i] = current[i] + png::paeth_predictor(0u, 0u, 0u);
  } // первый пиксель в строке не учитывает ничего

  for (auto i = m_pixel_bytesize; i < m_linebytes_width; ++i) {
    const auto recon_a = to[i - m_pixel_bytesize];

    to[i] = current[i] + png::paeth_predictor(recon_a, 0u, 0u); // нули замещают recon_b и recon_c
  }
}

} // namespace png
} // namespace csc
