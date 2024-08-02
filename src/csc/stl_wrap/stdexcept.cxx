module;
#include <stdexcept>
export module csc.stl_wrap.stdexcept;

export namespace csc {

using domain_error = std::domain_error;
using runtime_error = std::runtime_error;
using out_of_range = std::out_of_range;

} // namespace csc