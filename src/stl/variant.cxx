module;
#include <cstdint>
#include <variant>
export module stl.variant;

export namespace std {

using std::bad_variant_access;
using std::variant;
using std::operator==;
using std::operator<=>;
using std::add_pointer_t;
using std::get;
using std::get_if;
using std::holds_alternative;
using std::swap;
using std::variant_alternative_t;
using std::visit;

} // namespace std
