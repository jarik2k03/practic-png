#include "component.h"

namespace rrr {
class instance {
  component m_component;
  char m_c;
 public:
  instance() = default;
  char get_char() const  { return m_c; }
};

}

int main() {
  rrr::instance inst;
  rrr::component obj(inst);
}
