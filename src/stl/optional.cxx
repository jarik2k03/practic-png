module;
#include <cstdint>
#include <optional>
export module stl.optional;

export namespace std {

using std::make_optional;
using std::optional;

using std::bad_optional_access;
using std::hash;
using std::nullopt;
using std::swap;

using std::in_place;
using std::in_place_index;
using std::in_place_type;

using std::in_place_index_t;
using std::in_place_t;
using std::in_place_type_t;

using std::operator==;
using std::operator<=>;

} // namespace std
