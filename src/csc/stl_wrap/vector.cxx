module;
#include <vector>
export module csc.stl_wrap.vector;

export namespace csc {

template <typename Tp, typename Alloc = std::allocator<Tp>>
using vector = std::vector<Tp, Alloc>;
} // namespace csc