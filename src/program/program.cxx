#include <cstdlib>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#ifndef NDEBUG
import csc.png.picture_debug;
#endif

import csc.png;
import cstd.stl_wrap.string_view;
// import cstd.stl_wrap.string;
import cstd.stl_wrap.iostream;
import cstd.stl_wrap.stdexcept;

using cstd::operator<<;

csc::e_compression_level bring_compression_level(const char* arg) {
  using cstd::operator==;
  const std::string compr_level_str(arg);
  if (compr_level_str == "speed")
    return csc::e_compression_level::weakest;
  if (compr_level_str == "best")
    return csc::e_compression_level::strongest;
  if (compr_level_str.size() > 1)
    return csc::e_compression_level::default_;
  const char level = compr_level_str[0ul];
  if (level < '0' || level > '9')
    return csc::e_compression_level::default_;
  return static_cast<csc::e_compression_level>(level - '0');
}

csc::e_compression_strategy bring_strategy(const char* arg) {
  using cstd::operator==;
  const std::string strategy_str(arg);
  if (strategy_str == "huffman")
    return csc::e_compression_strategy::huffman_only;
  if (strategy_str == "filter")
    return csc::e_compression_strategy::filtered;
  if (strategy_str == "default")
    return csc::e_compression_strategy::default_;
  return csc::e_compression_strategy::default_;
}

int32_t bring_memory_usage(const char* arg) {
  const std::string usage_str(arg);
  if (usage_str.size() > 1)
    return 8;
  const char level = usage_str[0ul];
  if (level < '0' || level > '9')
    return 8;
  return static_cast<int32_t>(level - '0');
}

int32_t bring_window_bits(const char* arg) {
  const std::string bits_str(arg);
  int32_t level = std::stoi(bits_str);
  if (level < 8 || level > 15)
    return 15;
  return static_cast<int32_t>(level);
}

std::unordered_map<std::string, std::string> bring_options(const char* const* const argv, int32_t argc) {
  std::unordered_map<std::string, std::string> args;
  args.reserve(8u);
  for (uint32_t i = 1u; i < argc - 1u; i += 2u) {
    if (argv[i][0] == '-')
      args.insert({argv[i], argv[i + 1]});
  }
  return args;
}

auto check_valid_options(const std::unordered_map<std::string, std::string>& have) {
  const std::unordered_set<std::string> valid{"-i", "-o", "-compress", "-memory_usage", "-window-bits", "-strategy"};
  auto not_valid_option = [&valid](const auto& pair) -> bool { return !valid.contains(pair.first); };
  const auto not_valid_pos = std::ranges::find_if(have, not_valid_option);
  return not_valid_pos;
}

int main(int argc, char** argv) {
  using cstd::operator==;
  if (argc < 2) {
    cstd::cout << "Usage: " << argv[0] << " -i <input_png_file> -o [output_png_file]\n";
    cstd::cout << "Special options for \"output_png_file\":\n";
    cstd::cout << " -compress <0-9/speed/best> \n";
    cstd::cout << " -memory_usage <1-9> \n";
    cstd::cout << " -window_bits <8-15> \n";
    cstd::cout << " -strategy <huffman/filter/default> \n";
    std::exit(0);
  }

  try {
    const auto args = bring_options(argv, argc);
    const auto unrecognized_option_pos = check_valid_options(args);
    if (unrecognized_option_pos != args.cend())
      throw cstd::domain_error(std::string("Неопознанная опция программы: ") + unrecognized_option_pos->first);
    if ((argc & 0x1) == 0) // только нечетное кол-во аргументов: program(0) [-option(1) value(2)] [ ] ...
      throw cstd::domain_error("Аргументы не соответствуют шаблону: [-key value]");

    const auto i_pos = args.find("-i"), o_pos = args.find("-o");

    csc::picture file;
    csc::deserializer png_executor;
    if (i_pos != args.cend()) {
      file = png_executor.deserialize(i_pos->second);
    } else {
      throw cstd::domain_error("Не назначен входной файл!");
    }
#ifndef NDEBUG
    cstd::cout << file << '\n';
#endif
    if (o_pos != args.cend()) {
      const auto compress_pos = args.find("-compress"), memory_usage_pos = args.find("-memory_usage");
      const auto window_bits_pos = args.find("-window_bits"), strategy_pos = args.find("-strategy");

      auto compress = csc::e_compression_level::default_;
      auto memory_usage = 8, window_bits = 15;
      auto strategy = csc::e_compression_strategy::default_;

      if (compress_pos != args.cend())
        compress = bring_compression_level(compress_pos->second.c_str());
      if (memory_usage_pos != args.cend())
        memory_usage = bring_memory_usage(memory_usage_pos->second.c_str());
      if (window_bits_pos != args.cend())
        window_bits = bring_window_bits(window_bits_pos->second.c_str());
      if (strategy_pos != args.cend())
        strategy = bring_strategy(strategy_pos->second.c_str());

      csc::serializer png_writer;
      png_writer.serialize(o_pos->second, file, compress);
      cstd::cout << "Png saved to: " << o_pos->second << " with compression: " << static_cast<int32_t>(compress)
                 << '\n';
    }

  } catch (const cstd::exception& e) {
    cstd::cout << "Cannot process the file: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
