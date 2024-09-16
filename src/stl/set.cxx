module;
#include <set>
export module stl.set;

export namespace std {

using std::initializer_list;

using std::set;
using std::multiset;


using std::less;

using std::operator==;
using std::operator<=>;

using std::swap;

using std::erase_if;

namespace pmr {
using std::pmr::set;
using std::pmr::multiset;
}

} // namespace stl

