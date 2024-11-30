cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
project(practic-png LANGUAGES CXX VERSION 1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# zlib для inflate и deflate
find_package(ZLIB REQUIRED)
# vulkan для рендеринга на видеокарте
find_package(Vulkan)

# glsl-компилятор, который транслирует исходный код шейдеров в spir-v
find_program(glslc_exe NAMES glslc HINTS Vulkan::glslc)

add_subdirectory(shaders)


include(create_vk_module) # vulkan поддерживает c++ modules с v.1.3.256

find_package(glfw3 REQUIRED) # нужны колбэки событий и устранить утечку памяти из-за деструктора swapchain от xcb surface
find_package(glm REQUIRED) # матрицы и векторы

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS ON)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

add_compile_options(-Wall -Wextra -pedantic)


message(STATUS "Build type -- " ${CMAKE_BUILD_TYPE})

add_subdirectory(src)
add_subdirectory(external)

enable_testing()
