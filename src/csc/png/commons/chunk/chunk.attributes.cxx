module;
#include <cstdint>
export module csc.png.commons.chunk:attributes;

export namespace csc {
namespace png {

enum class e_section_code : uint32_t {
  success = 0u,
  error = 1u,
  depends_ihdr = 2u
};

} // namespace png
} // namespace csc
