include(png_short_paths)

add_library(${PIXEL_FORMATS} STATIC pixel_formats.lib.cxx)
target_sources(${PIXEL_FORMATS} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES pixel_formats.lib.cxx)
# target_link_libraries(${MEMORY_LITERALS} PRIVATE )

