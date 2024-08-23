#include <cstdlib>

import csc.png;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.iostream;
import cstd.stl_wrap.ios;
import cstd.stl_wrap.stdexcept;

using cstd::operator<<;
int main(int argc, char** argv) {
  csc::deserializer png_executor;
  if (argc < 2) {
    cstd::cout << "Usage: " << argv[0] << " <png_file>\n";
    std::exit(0);
  }

  try {
    const csc::picture file = png_executor.deserialize(static_cast<cstd::string_view>(argv[1]));
#ifndef NDEBUG
    cstd::cout << file << '\n';
#endif
  } catch (const cstd::exception& e) {
    cstd::cout << "Cannot process the file: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
