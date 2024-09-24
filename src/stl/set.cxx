module;
#include <set>
export module stl.set;

export namespace std {

using std::initializer_list;

using std::multiset;
using std::set;

using std::less;

using std::operator==;
using std::operator<=>;

using std::swap;

using std::erase_if;

namespace pmr {
using std::pmr::multiset;
using std::pmr::set;
} // namespace pmr

} // namespace std
