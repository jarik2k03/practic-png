#include <cstdlib>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#ifndef NDEBUG
import csc.png.picture_debug;
#endif

import csc.png;
import csc.pngine;
import stl.string_view;
import stl.string;
import stl.iostream;
import stl.stdexcept;

csc::e_compression_level bring_compression_level(const char* arg) {
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
  for (uint32_t i = 1u; i < argc - 1u; i += 2u) { // счет аргументов с argv[1]
    if (argv[i][0] == '-')
      args.insert({argv[i], argv[i + 1]});
  }
  return args;
}

auto check_valid_options(const std::unordered_map<std::string, std::string>& have) {
  const std::unordered_set<std::string> valid{
      "-i", "-o", "-compress", "-memory_usage", "-window_bits", "-strategy", "-force"};
  auto not_valid_option = [&valid](const auto& pair) -> bool { return !valid.contains(pair.first); };
  const auto not_valid_pos = std::ranges::find_if(have, not_valid_option);
  return not_valid_pos;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Использование: " << argv[0] << " -i <input_png_file> -o [output_png_file]\n";
    std::cout << "Дополнительные опции для \"input_png_file\":\n";
    std::cout << " -force <true> \n";
    std::cout << "Дополнительные опции для \"output_png_file\":\n";
    std::cout << " -compress <0-9/speed/best> \n";
    std::cout << " -memory_usage <1-9> \n";
    std::cout << " -window_bits <8-15> \n";
    std::cout << " -strategy <huffman/filter/default> \n";
    std::exit(0);
  }

  try {
    const auto args = bring_options(argv, argc);
    const auto unrecognized_option_pos = check_valid_options(args);
    if (unrecognized_option_pos != args.cend())
      throw std::invalid_argument(std::string("Неопознанная опция программы: ") + unrecognized_option_pos->first);
    if ((argc & 0x1) == 0) // только нечетное кол-во аргументов: program(0) [-option(1) value(2)] [ ] ...
      throw std::invalid_argument("Аргументы не соответствуют шаблону: [-key value]");

    const auto i_pos = args.find("-i"), o_pos = args.find("-o");

    csc::picture png;
    csc::deserializer png_executor;
    if (i_pos != args.cend()) {
      const auto force_pos = args.find("-force");
      const bool ignore_checksum = force_pos != args.end();
      csc::pngine::pngine core("PNG-viewer", csc::pngine::bring_version(1u, 3u, 256u));
      std::cout << "Движок: " << core.get_engine_name() << '\n';
      const auto vers = core.get_engine_version(), api = core.get_vk_api_version();
      std::cout << "Версия: " << vers.major << '.' << vers.minor << '.' << vers.patch << '\n';
      std::cout << "Версия выбранного VulkanAPI: " << api.major << '.' << api.minor << '.' << api.patch << '\n';
      std::cout << "Инициализация экземпляра Vulkan... \n";
      core.init_instance();
      std::cout << "Загрузка изображения в память...\n";
      png = png_executor.deserialize(i_pos->second, ignore_checksum);

    } else {
      throw std::invalid_argument("Не назначен входной файл!");
    }
#ifndef NDEBUG
    std::cout << png << '\n';
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
      png_writer.serialize(o_pos->second, png, compress, memory_usage, window_bits, strategy);
      //       std::getchar();
      std::cout << "Изображение успешно сохранено: " << o_pos->second
                 << " С уровнем сжатия: " << static_cast<int32_t>(compress) << '\n';
    }

  } catch (const std::runtime_error& e) {
    std::cout << "PNG-изображение не декодировано: \n - " << e.what() << '\n';
    std::exit(1);
  } catch (const std::invalid_argument& e) {
    std::cout << "Ошибка входных данных: \n - " << e.what() << '\n';
    std::exit(1);
  } catch (const std::domain_error& e) {
    std::cout << "Неправильная структура PNG: \n - " << e.what() << '\n';
    std::exit(1);
  } catch (const std::exception& e) {
    std::cout << "Ошибка: \n - " << e.what() << '\n';
    std::exit(1);
  }
}
