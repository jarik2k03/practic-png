add_subdirectory(sections)

add_library(png_t_signature OBJECT png_t.signature.cxx)
target_link_libraries(png_t_signature PRIVATE sections stl_array)
target_sources(png_t_signature PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png_t.signature.cxx ) 


add_library(png_t STATIC png_t.lib.cxx)
target_link_libraries(png_t PRIVATE sections png_t_signature)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
target_link_libraries(png_t PRIVATE stl_iostream stl_ios)
endif()

target_sources(png_t PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png_t.lib.cxx ) 

