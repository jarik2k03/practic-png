module;
#include <deque>
export module stl.deque;

export namespace std {

using std::initializer_list;

using std::deque;

using std::operator==;
using std::operator<=>;

using std::swap;

using std::erase;
using std::erase_if;

namespace pmr {
using std::pmr::deque;
}

} // namespace stl
