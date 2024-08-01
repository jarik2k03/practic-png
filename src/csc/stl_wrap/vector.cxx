module;
#include <vector>
export module csc.stl_wrap.vector;

export namespace csc {

template <typename Tp>
class basic_vector : public std::vector<Tp, std::allocator<Tp>> {};

template <typename Tp>
using vector = basic_vector<Tp>;
} // namespace csc