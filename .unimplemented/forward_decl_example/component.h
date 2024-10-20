#include <type_traits>

namespace rrr {
class instance;
template <typename T>
concept c_instance = std::is_same_v<T, rrr::instance>;

class component {
  char m_value;
 public:
  component() = default;
  template <c_instance instance>
  component(const instance& i) : m_value(i.get_char()) {}
};

}
