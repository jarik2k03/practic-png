module;
#include <vector>
export module stl.vector;

export namespace std {

using std::initializer_list;

using std::vector;

using std::operator==;
using std::operator<=>;

using std::swap;
using std::hash;

using std::erase;
using std::erase_if;

namespace pmr {
using std::pmr::vector;
}

} // namespace stl
