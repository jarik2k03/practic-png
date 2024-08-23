module;
#include <cstdint>
#include <variant>
export module cstd.stl_wrap.variant;

export namespace cstd {

template <typename... Types>
using variant = std::variant<Types...>;

using bad_variant_access = std::bad_variant_access;

#define VARIANT cstd::variant<Types...>

template <typename... Types>
constexpr bool operator==(const VARIANT& x, const VARIANT& y) noexcept {
  return std::operator==(x, y);
}
template <typename... Types>
constexpr auto operator<=>(const VARIANT& x, const VARIANT& y) noexcept {
  return std::operator<=>(x, y);
}

template <class... Types>
void swap(VARIANT& lhs, VARIANT& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  std::swap(lhs, rhs);
}

template <class Visitor, class... Variants>
constexpr auto visit(Visitor&& vis, Variants&&... vars) {
  return std::visit(vis, vars...);
}
template <class R, class Visitor, class... Variants>
constexpr R visit(Visitor&& vis, Variants&&... vars) {
  return std::visit(vis, vars...);
}

template <class T, class... Types>
constexpr bool holds_alternative(const std::variant<Types...>& v) noexcept {
  return std::holds_alternative<T>(v);
}

template <std::size_t I, typename Varnt>
using variant_alternative_t = std::variant_alternative_t<I, Varnt>;

template <std::size_t I, class... Types>
constexpr cstd::variant_alternative_t<I, cstd::variant<Types...>>& get(cstd::variant<Types...>& v) {
  return std::get<I>(v);
}
template <std::size_t I, class... Types>
constexpr cstd::variant_alternative_t<I, cstd::variant<Types...>>&& get(cstd::variant<Types...>&& v) {
  return std::get<I>(v);
}
template <std::size_t I, class... Types>
constexpr const cstd::variant_alternative_t<I, cstd::variant<Types...>>& get(const cstd::variant<Types...>& v) {
  return std::get<I>(v);
}
template <std::size_t I, class... Types>
constexpr const cstd::variant_alternative_t<I, cstd::variant<Types...>>&& get(const cstd::variant<Types...>&& v) {
  return std::get<I>(v);
}

template <class T, class... Types>
constexpr T& get(cstd::variant<Types...>& v) {
  return std::get<T>(v);
}
template <class T, class... Types>
constexpr T&& get(cstd::variant<Types...>&& v) {
  return std::get<T>(v);
}
template <class T, class... Types>
constexpr const T& get(const cstd::variant<Types...>& v) {
  return std::get<T>(v);
}
template <class T, class... Types>
constexpr const T&& get(const cstd::variant<Types...>&& v) {
  return std::get<T>(v);
}

template <typename IT>
using add_pointer_t = std::add_pointer_t<IT>;

#define INDEX cstd::variant_alternative_t<I, cstd::variant<Types...>>
#define TYPE T
template <std::size_t I, class... Types>
constexpr cstd::add_pointer_t<INDEX> get_if(cstd::variant<Types...>* pv) noexcept {
  return std::get_if<I>(pv);
}
template <std::size_t I, class... Types>
constexpr cstd::add_pointer_t<const INDEX> get_if(const cstd::variant<Types...>* pv) noexcept {
  return std::get_if<I>(pv);
}
template <class T, class... Types>
constexpr cstd::add_pointer_t<TYPE> get_if(cstd::variant<Types...>* pv) noexcept {
  return std::get_if<T>(pv);
}
template <class T, class... Types>
constexpr cstd::add_pointer_t<const TYPE> get_if(const cstd::variant<Types...>* pv) noexcept {
  return std::get_if<T>(pv);
}
} // namespace cstd