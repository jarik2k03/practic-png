add_library(pixel_formats STATIC pixel_formats.lib.cxx)
target_sources(pixel_formats PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES pixel_formats.lib.cxx)
# target_link_libraries(memory_literals PRIVATE )

