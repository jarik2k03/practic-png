module;
#include <type_traits>
export module csc.pngine.commons.forward.device;

export namespace csc {
namespace pngine {

class device_impl; // forward declaration

template <typename T>
concept FWDDevice = std::is_same_v<pngine::device_impl, T>;

} // namespace pngine
} // namespace csc
