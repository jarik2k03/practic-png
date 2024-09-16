module;
#include <stdexcept>
export module stl.stl_wrap.stdexcept;

export namespace std {
using exception = std::exception;

using std::domain_error;
using std::runtime_error;
using std::out_of_range;
using std::invalid_argument;
using std::length_error;
using std::logic_error;
using std::range_error;
using std::overflow_error;
using std::underflow_error;

} // namespace stl
