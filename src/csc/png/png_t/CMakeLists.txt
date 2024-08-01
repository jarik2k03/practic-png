add_subdirectory(sections)

add_library(png_t STATIC png_t.cxx)
target_link_libraries(png_t PRIVATE sections)

target_sources(png_t PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png_t.cxx ) 

