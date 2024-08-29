add_executable(program program.cxx)

target_include_directories(program PUBLIC ${PROJECT_SOURCE_DIR}/src)

set(CSC_PNG_BIN_PATH ${CMAKE_BINARY_DIR}/src/csc/png/CMakeFiles/png.dir)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(program PRIVATE -fmodules-ts)
    target_compile_options(program PRIVATE -fdeps-format=p1689r5)
    target_compile_options(program PRIVATE -fmodule-mapper=${CSC_PNG_BIN_PATH}/png.cxx.o.modmap)
endif()


target_link_libraries(program PRIVATE png stl_iostream stl_stdexcept)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  target_link_libraries(program PRIVATE picture_debug)
endif() 
