cmake_minimum_required(VERSION 3.26)
project(practic-png LANGUAGES CXX VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)


enable_testing()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "gcm.cache")


add_subdirectory(src)
add_subdirectory(external)
