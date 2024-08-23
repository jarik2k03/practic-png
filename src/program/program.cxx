#include <cstdlib>

import csc.png;
import csc.stl_wrap.string_view;
import csc.stl_wrap.iostream;
import csc.stl_wrap.stdexcept;

int main(int argc, char** argv) {
  csc::deserializer png_executor;
  if (argc < 2) {
    csc::cout << "Usage: " << argv[0] << " <png_file>\n";
    std::exit(0);
  }

  try {
    const csc::picture file = png_executor.deserialize(static_cast<csc::string_view>(argv[1]));
#ifndef NDEBUG
    csc::cout << file << '\n';
#endif
  } catch (const csc::exception& e) {
    csc::cout << "Cannot process the file: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
