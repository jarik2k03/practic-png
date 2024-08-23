add_subdirectory(png_t)
add_subdirectory(deserializer)
add_subdirectory(serializer)

add_library(png STATIC png.cxx)
target_link_libraries(png PUBLIC deserializer png_t serializer)
target_sources(png PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png.cxx ) 
