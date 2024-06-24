#add_subdirectory(png-serializer)
add_subdirectory(png-deserializer)
add_subdirectory(png_t)

add_library(png STATIC png.cxx)
target_link_libraries(png PUBLIC png-deserializer )

target_compile_options(png PRIVATE -fmodules-ts )
target_sources(png PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png.cxx ) 
