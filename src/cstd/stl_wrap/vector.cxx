module;
#include <vector>
export module stl.stl_wrap.vector;

export namespace std {

using std::vector;
namespace pmr {
using std::pmr::vector;
}

} // namespace stl
