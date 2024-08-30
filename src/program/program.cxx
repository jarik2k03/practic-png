#include <cstdlib>

#ifndef NDEBUG
import csc.png.picture_debug;
#endif

import csc.png;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.iostream;
import cstd.stl_wrap.stdexcept;

using cstd::operator<<;
int main(int argc, char** argv) {
  csc::deserializer png_executor;
  csc::serializer png_writer;
  if (argc < 2) {
    cstd::cout << "Usage: " << argv[0] << " <input_png_file> [output_png_file]\n";
    std::exit(0);
  }

  try {
    const cstd::string_view file_name = argv[1];
    const csc::picture file = png_executor.deserialize(file_name);
    if (argc == 3) {
      const cstd::string_view new_file_name = argv[2];
      png_writer.serialize(new_file_name, file);
      cstd::cout << "Png saved to: " << new_file_name.data() << '\n';
    }
#ifndef NDEBUG
    cstd::cout << file << '\n';
#endif
  } catch (const cstd::exception& e) {
    cstd::cout << "Cannot process the file: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
