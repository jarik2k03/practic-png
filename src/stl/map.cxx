module;
#include <map>
export module stl.map;

export namespace std {

using std::initializer_list;

using std::map;
using std::multimap;

using std::operator==;
using std::operator<=>;

using std::allocator;

using std::swap;

using std::erase_if;

namespace pmr {
using std::pmr::map;
using std::pmr::multimap;
} // namespace pmr
} // namespace std
