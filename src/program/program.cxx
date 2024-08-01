#include <iostream>

#include <bits/stl_construct.h>
import csc.png;
import csc.stl_wrap.string_view;

int main(int argc, char** argv) {
  csc::deserializer png_executor;
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <png_file>\n";
    std::exit(0);
  }

  try {
    const csc::png_t file = png_executor.deserialize(static_cast<csc::string_view>(argv[1]));
#ifndef NDEBUG
    std::cout << file << '\n';
#endif
  } catch (const std::exception& e) {
    std::cout << "Cannot process the file: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
