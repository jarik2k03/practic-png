module;
#include <stdexcept>
export module csc.stl_wrap.stdexcept;

export namespace csc {

class domain_error : public std::domain_error {
 public:
  domain_error(const char* msg) : std::domain_error(msg) {
  }
};
class runtime_error : public std::runtime_error {
 public:
  runtime_error(const char* msg) : std::runtime_error(msg) {
  }
};
class out_of_range : public std::out_of_range {
 public:
  out_of_range(const char* msg) : std::out_of_range(msg) {
  }
};
} // namespace csc