add_subdirectory(sections)

add_library(png_t STATIC png_t.cxx)
target_link_libraries(png_t PRIVATE sections)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
target_link_libraries(png_t PRIVATE stl_iostream stl_ios)
endif()

target_sources(png_t PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png_t.cxx ) 

