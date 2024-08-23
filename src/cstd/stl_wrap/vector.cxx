module;
#include <vector>
export module cstd.stl_wrap.vector;

export namespace cstd {

template <typename Tp, typename Alloc = std::allocator<Tp>>
using vector = std::vector<Tp, Alloc>;
} // namespace cstd