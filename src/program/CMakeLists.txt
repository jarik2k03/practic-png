add_executable(program program.cxx)

target_compile_options(program PRIVATE -fmodules-ts)
target_compile_options(program PRIVATE -fdeps-format=p1689r5)
target_include_directories(program PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_options(program PRIVATE -fmodule-mapper=${CMAKE_BINARY_DIR}/src/csc/png/CMakeFiles/png.dir/png.cxx.o.modmap)
target_link_libraries(program PRIVATE png)
