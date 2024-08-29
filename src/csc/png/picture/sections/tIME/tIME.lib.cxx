module;
#include <cstdint>
#include <ctime>
export module csc.png.picture.sections.tIME;

export namespace csc {

struct tIME {
  std::tm time_data{};
};

} // namespace csc
