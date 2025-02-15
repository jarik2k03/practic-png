#include <unistd.h>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#ifndef NDEBUG
import csc.png.picture_debug;
#endif

import csc.png;
import csc.pngine;
import csc.wnd;

import stl.string_view;
import stl.string;
import stl.iostream;
import stl.fstream;
import stl.ios;
import stl.stdexcept;

void switch_callbacks(csc::wnd::controller& states, csc::wnd::window_handler& event_ctl) {
  if (states.previous_state == states.current_state)
    return; // состояние не изменилось

  using namespace csc::wnd;
  if (states.previous_state == e_program_state::normal && states.current_state == e_program_state::insert) {
    event_ctl.set_mouse_button_callback(csc::wnd::applying_tool_by_mouse_event);
    event_ctl.set_key_callback(csc::wnd::applying_tool_by_keyboard_event);
    event_ctl.set_char_callback(csc::wnd::character_event);
  } else if (states.previous_state == e_program_state::insert && states.current_state == e_program_state::normal) {
    event_ctl.set_mouse_button_callback(csc::wnd::choosing_tool_by_mouse_event);
    event_ctl.set_key_callback(csc::wnd::choosing_tool_by_keyboard_event);
    event_ctl.set_char_callback(nullptr);
  }
}

csc::png::e_compression_level bring_compression_level(const char* arg) {
  const std::string compr_level_str(arg);
  if (compr_level_str == "speed")
    return csc::png::e_compression_level::weakest;
  if (compr_level_str == "best")
    return csc::png::e_compression_level::strongest;
  if (compr_level_str.size() > 1)
    return csc::png::e_compression_level::default_;
  const char level = compr_level_str[0ul];
  if (level < '0' || level > '9')
    return csc::png::e_compression_level::default_;
  return static_cast<csc::png::e_compression_level>(level - '0');
}

csc::png::e_compression_strategy bring_strategy(const char* arg) {
  const std::string strategy_str(arg);
  if (strategy_str == "huffman")
    return csc::png::e_compression_strategy::huffman_only;
  if (strategy_str == "filter")
    return csc::png::e_compression_strategy::filtered;
  if (strategy_str == "default")
    return csc::png::e_compression_strategy::default_;
  return csc::png::e_compression_strategy::default_;
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

    csc::png::picture png, menu;
    csc::png::deserializer png_executor;
    if (i_pos != args.cend()) {
      const auto force_pos = args.find("-force");
      const bool ignore_checksum = force_pos != args.end();
      png = png_executor.deserialize(i_pos->second, ignore_checksum);

      menu = png_executor.deserialize("./assets/buttons.png", true);
      png_executor.prepare_to_present(png); // здесь происходит второй этап декодирования изображения
      png_executor.prepare_to_present(menu);
      // движок на Vulkan для рендеринга картинки
      std::cout << "Инициализация экземпляра Vulkan... \n";
      csc::wnd::glfw_handler glfw_instance;
      csc::wnd::window_handler main_window(1280u, 720u, "PNG-обозреватель");

      csc::pngine::pngine_core core(
          "PNG-обозреватель", csc::pngine::bring_version(1u, 0u, 2u), main_window.get_required_instance_extensions());
      std::cout << "Движок: " << core.get_engine_name() << '\n';
      const auto vers = core.get_engine_version(), api = core.get_vk_api_version();
      std::cout << "Версия: " << vers.major << '.' << vers.minor << '.' << vers.patch << '\n';
      std::cout << "Версия выбранного VulkanAPI: " << api.major << '.' << api.minor << '.' << api.patch << '\n';
      std::cout << "Загрузка изображения в память...\n";
      core.setup_surface(main_window.create_surface(core.get_instance()));
      core.setup_gpu_and_renderer("AMD Radeon RX 6500 XT (RADV NAVI24)", main_window.get_framebuffer_size());

      csc::wnd::controller program_state;
      csc::wnd::pngine_picture_input_package pack_data{&core, &png, &program_state};
      main_window.set_size_limits({512u, 256u});
      main_window.set_user_pointer(&pack_data);
      main_window.set_framebuffer_size_callback(csc::wnd::resize_framebuffer_event);

      main_window.set_mouse_button_callback(csc::wnd::choosing_tool_by_mouse_event);
      main_window.set_key_callback(csc::wnd::choosing_tool_by_keyboard_event);
      main_window.set_char_callback(nullptr);

      core.change_drawing(png.m_image_data, png.header());
      core.init_menu(menu.m_image_data);
      core.load_mesh();

      double time = 0.0;
      uint64_t frames_count = 0u, fixed_frame_count = frames_count;
      while (!main_window.should_close()) {
        const auto start = std::chrono::high_resolution_clock::now();

        glfw_instance.poll_events();
        ::switch_callbacks(program_state, main_window);
        program_state.previous_state = program_state.current_state;

        core.render_frame();
        const auto end = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;
        frames_count += 1ul;
        // std::cout << "\033[H\033[2J";
        // std::cout << "Render frames per second: " << fixed_frame_count << '\n';
        // if (program_state.current_state == csc::wnd::e_program_state::insert)
        //   std::cout << "Input params string: " << program_state.input_data << '\n';

        [[unlikely]] if (time >= 1.0) {
          fixed_frame_count = frames_count;
          frames_count = 0u, time = 0.0;
        }
      }
    } else {
      throw std::invalid_argument("Не назначен входной файл!");
    }
#ifndef NDEBUG
    std::cout << png << '\n';
#endif
    if (o_pos != args.cend()) {
      const auto compress_pos = args.find("-compress"), memory_usage_pos = args.find("-memory_usage");
      const auto window_bits_pos = args.find("-window_bits"), strategy_pos = args.find("-strategy");

      auto compress = csc::png::e_compression_level::default_;
      auto memory_usage = 8, window_bits = 15;
      auto strategy = csc::png::e_compression_strategy::default_;

      if (compress_pos != args.cend())
        compress = bring_compression_level(compress_pos->second.c_str());
      if (memory_usage_pos != args.cend())
        memory_usage = bring_memory_usage(memory_usage_pos->second.c_str());
      if (window_bits_pos != args.cend())
        window_bits = bring_window_bits(window_bits_pos->second.c_str());
      if (strategy_pos != args.cend())
        strategy = bring_strategy(strategy_pos->second.c_str());

      csc::png::serializer png_writer;
      png_writer.serialize(o_pos->second, png, compress, memory_usage, window_bits, strategy);
      std::cout << "Изображение успешно сохранено: " << o_pos->second
                << " С уровнем сжатия: " << static_cast<int32_t>(compress) << '\n';
    }
  } catch (const std::runtime_error& e) {
    std::cout << "PNG-изображение не декодировано или ошибка рендеринга: \n - " << e.what() << '\n';
    std::exit(1);
  } catch (const std::invalid_argument& e) {
    std::cout << "Ошибка входных данных: \n - " << e.what() << '\n';
    std::exit(2);
  } catch (const std::domain_error& e) {
    std::cout << "Неправильная структура PNG: \n - " << e.what() << '\n';
    std::exit(3);
  } catch (const std::exception& e) {
    std::cout << "Ошибка: \n - " << e.what() << '\n';
    std::exit(9);
  }
}
