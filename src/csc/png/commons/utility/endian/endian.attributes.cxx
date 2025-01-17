module;
#include <cstdint>
#include <type_traits>
export module csc.png.commons.utility.endian:attributes;

export namespace csc {
namespace png {

template <typename T>
concept number = std::is_integral_v<T>;

}
} // namespace csc
