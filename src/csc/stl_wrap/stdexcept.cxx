module;
#include <stdexcept>
export module csc.stl_wrap.stdexcept;

export namespace csc {
using exception = std::exception;

using domain_error = std::domain_error;
using runtime_error = std::runtime_error;
using out_of_range = std::out_of_range;
using invalid_argument = std::invalid_argument;
using length_error = std::length_error;
using logic_error = std::logic_error;
using range_error = std::range_error;
using overflow_error = std::overflow_error;
using underflow_error = std::underflow_error;

} // namespace csc