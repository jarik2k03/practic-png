#include <cstdlib>

#ifndef NDEBUG
import csc.png.picture_debug;
#endif

import csc.png;
import cstd.stl_wrap.string_view;
import cstd.stl_wrap.string;
import cstd.stl_wrap.iostream;
import cstd.stl_wrap.stdexcept;

using cstd::operator<<;

csc::e_compression_level bring_compression_level(const char* arg) {
  const cstd::string compr_level_str(arg);
  if (compr_level_str.size() > 1)
    return csc::e_compression_level::default_;
  const char level = compr_level_str[0ul];
  if (level < '0' || level > '9')
    return csc::e_compression_level::default_;
  return static_cast<csc::e_compression_level>(level - '0');
}

int main(int argc, char** argv) {
  csc::deserializer png_executor;
  csc::serializer png_writer;
  if (argc < 2) {
    cstd::cout << "Usage: " << argv[0] << " <input_png_file> [output_png_file [compression]]\n";
    std::exit(0);
  }

  try {
    const cstd::string_view file_name = argv[1];
    const csc::picture file = png_executor.deserialize(file_name);
    if (argc >= 3) {
      const cstd::string_view new_file_name = argv[2];
      auto level = csc::e_compression_level::default_;
      if (argc == 4) {
        level = bring_compression_level(argv[3]);
      }
      png_writer.serialize(new_file_name, file, level);
      cstd::cout << "Png saved to: " << new_file_name.data() << " with compression: " << static_cast<int32_t>(level)
                 << '\n';
    }
#ifndef NDEBUG
    cstd::cout << file << '\n';
#endif
  } catch (const cstd::exception& e) {
    cstd::cout << "Cannot process the file: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
