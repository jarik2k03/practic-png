module;
#include <cstdint>
#include <variant>
export module stl.stl_wrap.variant;

export namespace std {

using std::variant;
using std::bad_variant_access;
using std::operator==;
using std::operator<=>;
using std::visit;
using std::swap;
using std::holds_alternative;
using std::variant_alternative_t;
using std::get;
using std::add_pointer_t;
using std::get_if;

} // namespace stl
